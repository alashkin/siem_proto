#include "stdafx.h"

#include <windows.h>
#include "ChangeGenerator1.h"
#include "EventUtils.h"
#include <algorithm>
#include "ChangeUtils.h"
#include "utils.h"
#include "SIEMWriter.h"

CChangeGenerator1::CChangeGenerator1(const Params& params) : m_params(params), m_createdChangesCount(0)
{
	m_params.eventCollector->SetEventsObject(this);
	m_waitObjects[woProceed].Attach(::CreateEvent(0, FALSE, TRUE, 0));
	m_waitObjects[woShutdown].Attach(::CreateEvent(0, TRUE, FALSE, 0));

	m_evtProceedCompleted.Attach(::CreateEvent(0, FALSE, FALSE, 0));

	m_dwWaitToProceedTime = INFINITE;
}


CChangeGenerator1::~CChangeGenerator1()
{
}

void CChangeGenerator1::Start()
{
	m_params.eventCollector->Start();

	std::thread workThread(&CChangeGenerator1::WorkFunc, this);

	m_workThread = std::move(workThread);
}

void CChangeGenerator1::SendStop()
{
	::SetEvent(m_waitObjects[woShutdown]);
	m_params.eventCollector->SendStop();
}

void CChangeGenerator1::WaitStop()
{
	m_workThread.join();

	m_params.eventCollector->WaitStop();
}

void CChangeGenerator1::OnNewEvent(EventUtils::RawEvent1& evt)
{
	m_sync1.Lock();
	boost::utils::scope_guard sg = boost::utils::make_guard([&](){ m_sync1.Unlock(); });

	std::shared_ptr<CEventProcessorBase> evtProc = m_params.eventProcessorSelector->GetProcessor(boost::get<UINT16>(evt.system->data[::EvtSystemEventID]));

	if (evtProc)
	{
		evtProc->Add(evt);
	}
}

DWORD CChangeGenerator1::ProceedEvents()
{
	m_sync1.Lock();
	boost::utils::scope_guard sg = boost::utils::make_guard([&](){ m_sync1.Unlock(); });

	CEventProcessorSelector::processors_vector_t& processors = m_params.eventProcessorSelector->GetProcessors();

	ChangeUtils::object_changes_t changes;

	time_t retWaitTime = 0;

	// converting events to changes 
	for (auto& proc : processors)
	{
		proc->Proceed();

		time_t waitTime = 0;
		proc->GetAndClearChanges(&changes, &waitTime);

		if (retWaitTime < waitTime)
		{
			retWaitTime = waitTime;
		}
	}

	for (auto& change : changes)
	{
		// parsing attributes
		m_params.attributeValuesParser->Parse(change.get());

		// dumping result
		++m_createdChangesCount;
		_LOG(debug) << L"@Changes: " << m_createdChangesCount;
		if ((m_createdChangesCount % 1000) == 0)
		{
			_LOG(info) << L"Changes created: " << m_createdChangesCount;
		}

		ChangeUtils::DumpObjectChange(*change.get());

		CSIEMWriter sw;
		sw.WriteEvent(*change.get());
	}

	return (retWaitTime == 0) ? INFINITE : ((DWORD)retWaitTime * 1000);
}

void CChangeGenerator1::OnTimeout()
{
	//	may be some diagnostic?
}

void CChangeGenerator1::OnChunkCompleted()
{
	::SetEvent(m_waitObjects[woProceed]); 
	::WaitForSingleObject(m_evtProceedCompleted, INFINITE);
}


void CChangeGenerator1::WorkFunc()
{
	while (true)
	{
		DWORD dwWaitRes = ::WaitForMultipleObjects(woCount, (HANDLE*)m_waitObjects, FALSE, m_dwWaitToProceedTime);
		if (dwWaitRes == WAIT_OBJECT_0 || dwWaitRes == WAIT_TIMEOUT)
		{
			m_dwWaitToProceedTime = ProceedEvents();
			::SetEvent(m_evtProceedCompleted);
		}
		else if (dwWaitRes == WAIT_OBJECT_0 + 1)
		{
			::SetEvent(m_evtProceedCompleted);
			break;
		}
	}
}