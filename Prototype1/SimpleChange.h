#pragma once

#include "ChangeBase.h"
#include "EventUtils.h"

class CSimpleChange : public CChangeBase
{
public:
	CSimpleChange(const EventUtils::ParsedEvent1& evt) : CChangeBase(evt) {};
	virtual ~CSimpleChange() {};
};