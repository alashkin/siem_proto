#pragma once

#include "ChangeUtils.h"
#include <vector>
#include <stdlib.h>
#include "EventUtils.h"
#include "ComplexChangeBase.h"

class CEDBasedObjectChange2 : public CComplexChangeBase
{
public:
	struct Params
	{
		unsigned int wait_for_event_min;
		unsigned int wait_for_event_max;
		unsigned int max_event_tolerance;
	};

	CEDBasedObjectChange2(const EventUtils::ParsedEvent1& evt, const Params& params);

	bool IsCompleted(time_t* timeToWait) const;

	void Add(const EventUtils::ParsedEvent1& evt);

	bool IsEventLinked(const EventUtils::ParsedEvent1& evt) const;

private:
	time_t m_createTime;
	time_t m_updateTime;

	void SelectAttributeValue(const EventUtils::ParsedEvent1& evt,
		ChangeUtils::ObjectChange::AttributeChange& attr,
		ChangeUtils::ObjectChange::attribute_values_t** valuesToAdd);

	void FillAttributeValue(ChangeUtils::ObjectChange::attribute_values_t* valuesToAdd,
		const EventUtils::ParsedEvent1& evt);

	void AddChangedAttributes(const EventUtils::ParsedEvent1& evt, ChangeUtils::ObjectChange* objectChange);

	Params m_params;

};
