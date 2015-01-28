#include "stdafx.h"
#include "ChangeBase.h"
#include "EventUtils.h"
#include "utils.h"
#include "DependedLogic.h"

CChangeBase::CChangeBase(const EventUtils::ParsedEvent1& evt) : m_objectChange(new ChangeUtils::ObjectChange())
{
	m_objectChange->changeType = DependedLogic::GetChangeType(ChangeUtils::ObjectChange::ChangeType::ctNone, evt);

	UpdateOrderFields(evt);
	FillChangeInfo(evt);
}

CChangeBase::~CChangeBase()
{
}

void CChangeBase::UpdateOrderFields(const EventUtils::ParsedEvent1& evt)
{
	if ((m_objectChange->changeInfo.meta.timeCreated > boost::get<UINT64>(evt.system->data[::EvtSystemTimeCreated])) 
		||
		(m_objectChange->changeInfo.meta.timeCreated == boost::get<UINT64>(evt.system->data[::EvtSystemTimeCreated])) &&
		(m_objectChange->changeInfo.meta.eventRecordID > boost::get<UINT64>(evt.system->data[::EvtSystemEventRecordId])))
	{
		m_objectChange->changeInfo.meta.eventRecordID = boost::get<UINT64>(evt.system->data[::EvtSystemEventRecordId]);
		m_objectChange->changeInfo.meta.timeCreated = boost::get<UINT64>(evt.system->data[::EvtSystemTimeCreated]);
	}
}

void CChangeBase::FillChangeInfo(const EventUtils::ParsedEvent1& evt)
{
	DependedLogic::LoadChangeInfo(m_objectChange->changeType, evt, m_objectChange.get());
}

std::shared_ptr<ChangeUtils::ObjectChange> CChangeBase::GetObjectChange()
{
	return m_objectChange;
}
