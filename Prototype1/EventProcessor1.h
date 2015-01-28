#pragma once

#include <Windows.h>
#include <winevt.h>
#include <string>
#include <map>
#include "EventParser.h"
#include <vector>
#include "EventUtils.h"

class CEventProcessor1
{
public:
	
	struct Event
	{
		EventUtils::EventSystemProperties system;
		CEventParser::event_props_t userData;
		CEventParser::event_props_t eventData;
	};

	void AddEvent(Event& evt);
	void ProcessEvents();

	CEventProcessor1();
	virtual ~CEventProcessor1();

private:
	std::vector<Event> m_events;

};

