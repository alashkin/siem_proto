#pragma once

#include "Config.h"
#include <boost/scoped_array.hpp>
#include <Windows.h>
#include <winevt.h>
#include <atlbase.h>
#include <thread>
#include <memory>
#include "XMLQueryGen.h"
#include "utils.h"
#include "XMLQueryGenBase.h"
#include "EventProcessor2.h"
#include "GeneratorIntf.h"
#include "EvtHandle.h"

class CEventCollector1 : public EventsObject<IEventCollectorEvents>
{
public:
	using EventsObject<IEventCollectorEvents>::SetEventsObject;

	struct Params
	{
		std::shared_ptr<Config::Target> target;
		std::shared_ptr<Config::IBookmarkStorage> bkmStg;
		std::shared_ptr<IXMLQueryGen> xmlQueryGen;
	};

	void Start();
	void SendStop();
	void WaitStop();

	CEventCollector1(Params& params);
	virtual ~CEventCollector1();

private:
	Params m_params;

	std::wstring m_query; // cached query

	CEvtHandle m_hEvtCurrentPos;
	CEvtHandle m_hEvtSystemRenderContext;

	ATL::CHandle m_waitObjects[2];
	enum WaitObjects { woExit = 0, woNewEvents = 1, woCount = 2 };
	std::thread m_collectThread;
	unsigned int m_collectedEventsCount;
	void CollectThread();
	void Execute(const EVT_HANDLE& hEvtResults, const EVT_HANDLE& hBookmark);

	// "consts"
	unsigned int m_chunkSize;

	void ConnectToServer(CEvtHandle* hEvtSession);
	void CheckConnection(CEvtHandle& hEvtSession);
};

