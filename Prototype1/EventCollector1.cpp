#include "stdafx.h"

#include "EventCollector1.h"
#include "Log.h"
#include <pugixml.hpp>
#include "unicode.h"
#include <boost/lexical_cast.hpp>
#include <sstream>
#include <atlbase.h>
#include <winevt.h>
#include <vector>
#include "utils.h"
#include <thread>
#include "EvtConvert.h"
#include "EventUtils.h"


CEventCollector1::CEventCollector1(Params& params) : m_params(params), m_collectedEventsCount(0)
{
	m_query = m_params.xmlQueryGen->GetQuery();
	m_chunkSize = m_params.target->initial_chunk_size;
}


CEventCollector1::~CEventCollector1()
{
}

void CEventCollector1::ConnectToServer(CEvtHandle* hEvtSession)
{
	EVT_RPC_LOGIN loginInfo;
	loginInfo.Domain = const_cast<LPWSTR>(m_params.target->domain.c_str());
	loginInfo.Server = const_cast<LPWSTR>(m_params.target->server.c_str());
	loginInfo.User = const_cast<LPWSTR>(m_params.target->user.c_str());
	loginInfo.Password = const_cast<LPWSTR>(m_params.target->password.c_str());
	loginInfo.Flags = m_params.target->loginType;

	hEvtSession->CloseAndReassign(
		::EvtOpenSession(EvtRpcLogin, &loginInfo, 0, 0));
	hEvtSession->Check("EvtOpenSession");
}

void CEventCollector1::CheckConnection(CEvtHandle& hEvtSession)
{
	_LOG(debug) << "Checking connection to " << m_params.target->server;

	LPWSTR pszQuery = L"<QueryList> <Query Path='application'>" \
		L"<Select>Event/System[EventID=9999999]</Select>" \
		L"  </Query> </QueryList>";

	CEvtHandle hEvtCheck(::EvtQuery(hEvtSession, 0, pszQuery, EvtQueryChannelPath));
	hEvtCheck.Check("EvtQuery:Ping");
}

void CEventCollector1::CollectThread()
{
	m_waitObjects[WaitObjects::woExit].Attach(::CreateEvent(NULL, TRUE, FALSE, NULL));
	m_waitObjects[WaitObjects::woNewEvents].Attach(::CreateEvent(NULL, FALSE, TRUE, NULL));

	while (::WaitForSingleObject(m_waitObjects[WaitObjects::woExit], 0) == WAIT_TIMEOUT)
	{
		::SetEvent(m_waitObjects[WaitObjects::woNewEvents]);

		try
		{
			_LOG(info) << "Collect thread initialized.";

			CEvtHandle hEvtSession;

			if (!m_params.target->server.empty())
			{
				_LOG(info) << L"Connecting to " << m_params.target->server;
				ConnectToServer(&hEvtSession);
				CheckConnection(hEvtSession);
			}
			else
			{
				_LOG(debug) << "Using localhost as target";
			}

			std::wstring xmlBookmark = m_params.bkmStg->Load();

			_LOG(debug) << "Creating bookmark " << (xmlBookmark.empty() ? L"(new)" : xmlBookmark) << "...";

			CEvtHandle hEvtBookmark = ::EvtCreateBookmark(xmlBookmark.empty() ? 0 : xmlBookmark.c_str());
			hEvtBookmark.Check("EvtCreateBookmark");

			_LOG(debug) << "Creating SYSTEM render context";
			m_hEvtSystemRenderContext.CloseAndReassign(::EvtCreateRenderContext(0, 0, EvtRenderContextSystem));
			m_hEvtSystemRenderContext.Check("EvtCreateRenderContext (SYSTEM)");

			_LOG(debug) << "Subscribing to query...";

			CEvtHandle hEvtSubscribe = ::EvtSubscribe(hEvtSession, m_waitObjects[WaitObjects::woNewEvents], nullptr,
				m_query.c_str(), xmlBookmark.empty()? nullptr : (EVT_HANDLE)hEvtBookmark, 
					nullptr, nullptr, xmlBookmark.empty()? EvtSubscribeStartAtOldestRecord : EvtSubscribeStartAfterBookmark);
			hEvtSubscribe.Check("EvtSubscribe");

			while (true)
			{
				_LOG(debug) << "Waiting...";
				DWORD res = ::WaitForMultipleObjects(WaitObjects::woCount,
					(HANDLE*)m_waitObjects, FALSE, m_params.target->network_timeout * 1000 );
				if (res == WAIT_OBJECT_0)
				{
					_LOG(info) << "\"Shutdown\" event received";
					break;
				}
				else if (res == WAIT_OBJECT_0 + 1)
				{
					_LOG(debug) << "\"New events\" signal received";
					Execute(hEvtSubscribe, hEvtBookmark);

					wchar_t bkmRendered[32 * 1024] = { 0 };
					DWORD bufferUsed, propertiesCount;

					_LOG(debug) << "Rendering bookmark...";
					if (!::EvtRender(nullptr, hEvtBookmark, EvtRenderBookmark,
						sizeof(bkmRendered), &bkmRendered[0], &bufferUsed, &propertiesCount))
					{
						throw std::runtime_error(utils::FormatLastErrorA("EvtRender"));
					}

					_LOG(debug) << "Saving bookmark..." << std::wstring(bkmRendered);
					m_params.bkmStg->Save(bkmRendered);
				}
				else if (res == WAIT_TIMEOUT)
				{
					GetEventsObject()->OnTimeout();
					CheckConnection(hEvtSession);
				}
				else
				{
					throw std::runtime_error(utils::FormatLastErrorA("WaitForMultipleObjects"));
				}
			}

			_LOG(debug) << "Collect thread exiting";
		}
		catch (std::exception& e)
		{
			_LOG(error) << boost::lexical_cast<std::wstring>(e) << ". Restarting collect thread in " << m_params.target->network_timeout << " seconds.";
			::WaitForSingleObject(m_waitObjects[WaitObjects::woExit],m_params.target->network_timeout * 1000);
		}
	}
}

void CEventCollector1::Start()
{
	_LOG(debug) << "Starting collect thread...";
	m_collectedEventsCount = 0;
	std::thread collectThread(&CEventCollector1::CollectThread, this);

	m_collectThread = std::move(collectThread);
}

void CEventCollector1::SendStop()
{
	_LOG(debug) << "Fire shutdown event to thread " << m_collectThread.get_id();

	::SetEvent(m_waitObjects[WaitObjects::woExit]);
}
void CEventCollector1::WaitStop()
{
	_LOG(debug) << "Waiting collect thread id:" << m_collectThread.get_id();
	m_collectThread.join();
}

void CEventCollector1::Execute(const EVT_HANDLE& hEvtResults, const EVT_HANDLE& hBookmark)
{
	_LOG_PREPARE_REC();

	DWORD returned = 0;

	while (true)
	{
		std::vector<CEvtHandle> events(m_chunkSize);

		_LOG(debug) << "Loading chunk; size=" << m_chunkSize;
		if (!::EvtNext(hEvtResults, events.size(), (EVT_HANDLE*)events.data(), INFINITE, 0, &returned))
		{
			if (::GetLastError() != ERROR_NO_MORE_ITEMS)
			{
				throw std::runtime_error(utils::FormatLastErrorA("EvtNext"));
			}
			_LOG(debug) << "There aren't new events";
			break;
		}

		_LOG(debug) << "Loaded chunk (" << returned << " events)";

		for (DWORD i = 0; i < returned; i++)
		{
			++m_collectedEventsCount;
			_LOG(info) << "@Events: " << m_collectedEventsCount;
			if ((m_collectedEventsCount % 1000) == 0)
			{
				_LOG(info) << "Events collected: " << m_collectedEventsCount;
			}
			DWORD bufSize;
			DWORD propertiesCount;

			::EvtRender(0, events[i], EvtRenderEventXml, 0, 0, &bufSize, &propertiesCount);
			if (::GetLastError() != ERROR_INSUFFICIENT_BUFFER)
			{
				throw std::runtime_error(utils::FormatLastErrorA("EvtRender (MEMORY/SYSTEM)"));
			}

			std::shared_ptr<std::wstring> userProperties(new std::wstring(bufSize/sizeof(wchar_t), 0));

			_LOG(debug) << "Rendering event to XML";
			if (!::EvtRender(0, events[i], EvtRenderEventXml, bufSize, 
					(PVOID)userProperties->data(), &bufSize, &propertiesCount))
			{
				throw std::runtime_error(utils::FormatLastErrorA("EvtRender (to XML)"));
			}

			::EvtRender(m_hEvtSystemRenderContext, events[i], EvtRenderEventValues, 0, 0, &bufSize, &propertiesCount);
			if (::GetLastError() != ERROR_INSUFFICIENT_BUFFER)
			{
				throw std::runtime_error(utils::FormatLastErrorA("EvtRender (MEMORY/CONTEXT)"));
			}

			std::vector<BYTE> systemProperties(bufSize);

			_LOG(debug) << "Rendering SYSTEM properties";
			if (!::EvtRender(m_hEvtSystemRenderContext, events[i], EvtRenderEventValues,
				bufSize, systemProperties.data(), &bufSize, &propertiesCount))
			{
				throw std::runtime_error(utils::FormatLastErrorA("EvtRender (CONTEXT)"));
			}

			std::shared_ptr<EventUtils::EventSystemProperties> systemPropertiesUnwrapped =
								EventUtils::UnwrapSystemProperties((EVT_VARIANT*)systemProperties.data());

			EventUtils::RawEvent1 evt;
			evt.system = systemPropertiesUnwrapped;
			evt.eventXML = userProperties;
			GetEventsObject()->OnNewEvent(evt);

			_LOG(debug) << "Render OK";
		}

		_LOG(debug) << "Updating bookmark";
		if (!::EvtUpdateBookmark(hBookmark, events[returned - 1]))
		{
			throw std::runtime_error(utils::FormatLastErrorA("EvtUpdateBookmark"));
		}

		GetEventsObject()->OnChunkCompleted();

	}

}
