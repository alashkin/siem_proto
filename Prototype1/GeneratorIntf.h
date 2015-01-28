#pragma once

#include "EventUtils.h"
#include <WinEvt.h>
#include <memory>
#include <string>

template <class Events> class EventsObject
{
private:
	Events* m_events;

protected:
	void SetEventsObject(Events* events)
	{
		m_events = events;
	}

	Events* GetEventsObject()
	{
		return m_events;
	}
};

class IEventCollectorEvents
{
public:
	virtual void OnNewEvent(EventUtils::RawEvent1& ) = 0;
	virtual void OnChunkCompleted() = 0;
	virtual void OnTimeout() = 0;
};