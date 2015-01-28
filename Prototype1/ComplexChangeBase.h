#pragma once

#include "ChangeBase.h"

class CComplexChangeBase : public CChangeBase
{
public:
	CComplexChangeBase(const EventUtils::ParsedEvent1& evt);
	virtual ~CComplexChangeBase();
};

