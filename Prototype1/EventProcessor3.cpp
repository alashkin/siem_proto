#include "stdafx.h"
#include "EventProcessor3.h"
#include "EventParser.h"

CEventProcessor3::CEventProcessor3()
{
}


CEventProcessor3::~CEventProcessor3()
{
}

void CEventProcessor3::Proceed()
{
	for (auto& evt : m_events)
	{
		auto new_change = std::make_shared<CEDBasedObjectChange3>(evt);
		m_objectChanges.push_back(new_change);
	}

	m_events.clear();
}

void CEventProcessor3::Add(EventUtils::RawEvent1& evt)
{
	CEventParser ep;
	ep.Parse(evt.eventXML->c_str(), evt.eventXML->size()*sizeof(wchar_t));

	EventUtils::ParsedEvent1 pe;
	pe.system = evt.system;
	pe.eventData = ep.GetEventData();

	EventUtils::DumpEvent(pe);

	m_events.push_back(pe);
}

void CEventProcessor3::GetAndClearChanges(ChangeUtils::object_changes_t* objectChanges)
{
	for (auto& change : m_objectChanges)
	{
		objectChanges->push_back(change->GetObjectChange());
	}

	m_objectChanges.clear();
}
