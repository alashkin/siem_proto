#pragma once

#include "SimpleChange.h"
#include "GenericSimpleProcessor.h"
#include "EventWithFlatEventDataParser.h"
#include "ChangeUtils.h"
#include "EventUtils.h"

//+++++

namespace details 
{ 
	class CObjectMovedChange;
};

typedef CGenericSimpleEventProcessor<details::CObjectMovedChange, CEventWithFlatEventDataParser> CObjectMovedProcessor;

//-----

namespace details
{
	class CObjectMovedChange : public CSimpleChange
	{
	public:
		CObjectMovedChange(EventUtils::ParsedEvent1& evt) : CSimpleChange(evt)
		{
			ChangeUtils::ObjectChange::AttributeChange change;
			ChangeUtils::ObjectChange::AttributeValue oldValue, newValue;
			oldValue.rawValue = evt.eventData->at(EventUtils::Consts::EventData::OldDN);
			newValue.rawValue = evt.eventData->at(EventUtils::Consts::EventData::NewDN);

			change.oldValues.insert(oldValue);
			change.newValues.insert(newValue);

			m_objectChange->changedAttributes.insert(std::make_pair(EventUtils::Consts::EventData::WHAT_DN, change));
		}

		virtual ~CObjectMovedChange()
		{

		}

	};
}
