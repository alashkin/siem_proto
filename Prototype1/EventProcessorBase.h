#pragma once
#include "EventUtils.h"
#include "ChangeUtils.h"
#include <vector>

class CEventProcessorBase
{
public:
	typedef std::vector<EventUtils::ParsedEvent1> parsed_events_t;

	virtual void Proceed() = 0;
	virtual void Add(EventUtils::RawEvent1& evt) = 0;
	virtual void GetAndClearChanges(ChangeUtils::object_changes_t* objectChanges, time_t* timeToWait) = 0;

	CEventProcessorBase();
	virtual ~CEventProcessorBase();
};

