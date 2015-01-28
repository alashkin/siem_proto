#include "stdafx.h"
#include "EventProcessor2.h"
#include <set>
#include <algorithm>
#include <Windows.h>
#include <string>
#include <time.h>
#include <stdlib.h>
#include "EventUtils.h"
#include "EventWithFlatEventDataParser.h"

void CEventProcessor2::Proceed()
{	
	for (auto& evt : m_events)
	{
		auto change = std::find_if(m_objectChanges.begin(), m_objectChanges.end(),
				[&evt](std::shared_ptr<CEDBasedObjectChange2> change) -> bool { return change->IsEventLinked(evt); });

		if (change == m_objectChanges.end())
		{
			// there is no appropriate change, creating a new one
			CEDBasedObjectChange2::Params params;
			params.max_event_tolerance = m_params.max_event_tolerance;
			params.wait_for_event_min = m_params.wait_for_event_min;
			params.wait_for_event_max = m_params.wait_for_event_max;

			auto new_change = std::make_shared<CEDBasedObjectChange2>(evt, params);
			m_objectChanges.push_back(new_change);
		}
		else
		{
			// adding event to the existing change
			(*change)->Add(evt);
		}
	}

	m_events.clear();
}

void CEventProcessor2::GetAndClearChanges(ChangeUtils::object_changes_t* objectChanges, time_t* waitTime)
{
	auto change = m_objectChanges.begin();
	while (change != m_objectChanges.end())
	{
		time_t changeWaitTime;
		if (change->get()->IsCompleted(&changeWaitTime))
		{
			objectChanges->push_back(change->get()->GetObjectChange());
			change = m_objectChanges.erase(change);
		}
		else
		{
			if (*waitTime < changeWaitTime)
			{
				*waitTime = changeWaitTime;
			}

			change++;
		}
	}

	_LOG(debug) << L"EventProcessorWait: " << *waitTime << L" sec., Changes remaining: " << m_objectChanges.size();

}

void CEventProcessor2::Add(EventUtils::RawEvent1& evt)
{
	EventUtils::ParsedEvent1 pe;
	pe.system = evt.system;
	pe.eventData = CEventWithFlatEventDataParser::Parse(evt);

	EventUtils::DumpEvent(pe);

	m_events.push_back(pe);
}

CEventProcessor2::CEventProcessor2(Params& params) : m_params(params)
{
}


CEventProcessor2::~CEventProcessor2()
{
}
