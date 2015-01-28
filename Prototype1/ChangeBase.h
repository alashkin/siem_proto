#pragma once

#include "EventUtils.h"
#include "ChangeUtils.h"
#include <memory>

class CChangeBase
{
public:
	CChangeBase(const EventUtils::ParsedEvent1& evt);
	virtual ~CChangeBase();

	std::shared_ptr<ChangeUtils::ObjectChange> GetObjectChange();

protected:
	std::shared_ptr<ChangeUtils::ObjectChange> m_objectChange;

	void UpdateOrderFields(const EventUtils::ParsedEvent1& evt);
	void FillChangeInfo(const EventUtils::ParsedEvent1& evt);
};

