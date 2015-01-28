#include "stdafx.h"

#include "EDBasedObjectChange2.h"
#include "DependedLogic.h"
#include "utils.h"

CEDBasedObjectChange2::CEDBasedObjectChange2(const EventUtils::ParsedEvent1& evt, const Params& params) : CComplexChangeBase(evt), m_params(params)
{
	m_createTime = time(0);

	Add(evt);
}


void CEDBasedObjectChange2::SelectAttributeValue(const EventUtils::ParsedEvent1& evt,
												ChangeUtils::ObjectChange::AttributeChange& attr,
												ChangeUtils::ObjectChange::attribute_values_t** valuesToAdd)
{
	ChangeUtils::ObjectChange::AttributeValue* retVal = 0;

	if (evt.eventData->at(EventUtils::Consts::EventData::OperationType) == EventUtils::Consts::Values::OpTypeAdded)
	{
		// added - new value
		*valuesToAdd = &attr.newValues;
	}
	else if (evt.eventData->at(EventUtils::Consts::EventData::OperationType) == EventUtils::Consts::Values::OpTypeDeleted)
	{
		*valuesToAdd = &attr.oldValues;
	}
	else
	{
		//TODO: SOS!!! Now returning NULL and cousing Access Violation ;)
	}
}

void CEDBasedObjectChange2::FillAttributeValue(ChangeUtils::ObjectChange::attribute_values_t* valuesToAdd,
	const EventUtils::ParsedEvent1& evt)
{
	ChangeUtils::ObjectChange::AttributeValue newValue;
	newValue.meta.eventRecordID = boost::get<UINT64>(evt.system->data[::EvtSystemEventRecordId]);
	newValue.meta.timeDetected = boost::get<UINT64>(evt.system->data[::EvtSystemTimeCreated]);
	newValue.rawValue = evt.eventData->at(EventUtils::Consts::EventData::AttributeValue);
	valuesToAdd->insert(newValue);
}

void CEDBasedObjectChange2::AddChangedAttributes(const EventUtils::ParsedEvent1& evt, ChangeUtils::ObjectChange* objectChange)
{
	auto& attrName = evt.eventData->find(EventUtils::Consts::EventData::AttributeName);

	if (attrName != evt.eventData->end())
	{
		auto& attrChange = objectChange->changedAttributes.find(attrName->second);
		if (attrChange == objectChange->changedAttributes.end())
		{
			// don't have such attribute change yet
			ChangeUtils::ObjectChange::AttributeChange newChange;
			ChangeUtils::ObjectChange::attribute_values_t *valuedToAdd;
			SelectAttributeValue(evt, newChange, &valuedToAdd);

			FillAttributeValue(valuedToAdd, evt);
			objectChange->changedAttributes.insert(std::make_pair(attrName->second, newChange));
		}
		else
		{
			// have this attribute change already
			ChangeUtils::ObjectChange::attribute_values_t *valuesToAdd;
			SelectAttributeValue(evt, attrChange->second, &valuesToAdd);
			FillAttributeValue(valuesToAdd, evt);
		}
	}
}


bool CEDBasedObjectChange2::IsCompleted(time_t* timeToWait) const
{
	time_t now = time(0);
	bool bRetVal =  ((now - m_updateTime) >= m_params.wait_for_event_min ||
				     (now - m_createTime) >= m_params.wait_for_event_max);

	if (!bRetVal)
	{
		*timeToWait = now - m_updateTime + m_params.wait_for_event_min;
	}

	return bRetVal;
}

void CEDBasedObjectChange2::Add(const EventUtils::ParsedEvent1& evt)
{
	m_updateTime = time(0);
	m_objectChange->changeType = DependedLogic::GetChangeType(m_objectChange->changeType, evt);
	UpdateOrderFields(evt);
	AddChangedAttributes(evt, m_objectChange.get());
}

bool CEDBasedObjectChange2::IsEventLinked(const EventUtils::ParsedEvent1& evt) const
{
	if (DependedLogic::AcceptEvent(m_objectChange->changeType, evt))
	{
		if ((m_objectChange->changeInfo.meta.CID // by CorrelationID
			== evt.eventData->at(EventUtils::Consts::EventData::CID))
			||
			(  	// WHAT_GUID + WHO_DOMAIN + WHO_NAME + TIME+- maxToleranceTime
			(DependedLogic::AllowToleranceMatching(m_objectChange->changeType)) &&
			(m_objectChange->changeInfo.data[ChangeUtils::Consts::ChangeInfo::WHAT_Guid] == evt.eventData->at(EventUtils::Consts::EventData::WHAT_Guid)) &&
			(m_objectChange->changeInfo.data[ChangeUtils::Consts::ChangeInfo::WHO_DomainNetbios] == evt.eventData->at(EventUtils::Consts::EventData::WHO_DomainNetbios)) &&
			(m_objectChange->changeInfo.data[ChangeUtils::Consts::ChangeInfo::WHO_Name] == evt.eventData->at(EventUtils::Consts::EventData::WHO_Name)) &&
			(abs((long long)(m_objectChange->changeInfo.meta.timeCreated -
				boost::get<UINT64>(evt.system->data[::EvtSystemTimeCreated]))) / 10000000  < m_params.max_event_tolerance)
			))
		{
			return true;
		}
	}

	return false;
}

