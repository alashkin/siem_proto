#pragma once
#include "AttributeValueParserBase.h"

class CGroupTypeParser : public CAttributeValueParserBase
{
public:
	CGroupTypeParser();
	virtual ~CGroupTypeParser();

	virtual void Parse(const std::wstring& , ChangeUtils::ObjectChange::AttributeChange* attrChange, const ChangeUtils::ObjectChange&) override;

private:

	std::wstring ParseOneValue(const std::wstring& attrValue);

};

