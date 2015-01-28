#pragma once

#include "EventCollector1.h"
#include "EventProcessor2.h"
#include <memory>
#include "GeneratorIntf.h"
#include <atlbase.h>
#include <thread>
#include "EventProcessorSelector.h"
#include "AttributeValuesParser.h"

// Handling ONE target

class CChangeGenerator1 : public IEventCollectorEvents
{
public:
	struct Params
	{
		std::shared_ptr<CEventCollector1> eventCollector;
		// eventFilter
		std::shared_ptr<CEventProcessorSelector> eventProcessorSelector;
		std::shared_ptr<CAttributeValuesParser>  attributeValuesParser;
		// changeFilter
	};

	void Start();
	void SendStop();
	void WaitStop();
	
	CChangeGenerator1(const Params& params);
	virtual ~CChangeGenerator1();

private:
	Params m_params;
	unsigned int m_createdChangesCount;

	virtual void OnNewEvent(EventUtils::RawEvent1& evt) override;
	virtual void OnChunkCompleted() override;
	virtual void OnTimeout() override;

	DWORD ProceedEvents();

	CEventProcessor2::parsed_events_t m_events;

	enum WaitObjects { woProceed, woShutdown, woCount };
	CHandle m_waitObjects[2];
	CHandle m_evtProceedCompleted;

	CComAutoCriticalSection m_sync1;
	DWORD m_dwWaitToProceedTime;
	std::thread m_workThread;
	void WorkFunc();
};

