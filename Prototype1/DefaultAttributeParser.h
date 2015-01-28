#pragma once
#include "AttributeValueParserBase.h"
#include "ADSchema.h"
#include <memory>

class CDefaultAttributeParser : public CAttributeValueParserBase
{
public:
	void Parse(const std::wstring& attrName, ChangeUtils::ObjectChange::AttributeChange* attrChange, const ChangeUtils::ObjectChange&);

	CDefaultAttributeParser(std::shared_ptr<CADSchema> adSchema);
	virtual ~CDefaultAttributeParser();

private:
	std::shared_ptr<CADSchema> m_adSchema;

};

