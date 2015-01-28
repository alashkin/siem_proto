#pragma once

#include "AttributeValueParserBase.h"
#include <string>

class CUACParser : public CAttributeValueParserBase
{
public:
	CUACParser();
	virtual ~CUACParser();

	void Parse(const std::wstring& , ChangeUtils::ObjectChange::AttributeChange* attrChange, const ChangeUtils::ObjectChange&) override;

	std::wstring ParseOneValue(UINT32 value);

};

