#pragma once

#include <string>
#include "ChangeUtils.h"

class CAttributeValueParserBase
{
public:
	CAttributeValueParserBase() {};
	virtual ~CAttributeValueParserBase() {};

	virtual void Parse(const std::wstring& attrName, ChangeUtils::ObjectChange::AttributeChange* attrChange, const ChangeUtils::ObjectChange& ) = 0;
};