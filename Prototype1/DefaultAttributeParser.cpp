#include "stdafx.h"
#include "DefaultAttributeParser.h"


CDefaultAttributeParser::CDefaultAttributeParser(std::shared_ptr<CADSchema> adSchema) : m_adSchema(adSchema)
{
}


CDefaultAttributeParser::~CDefaultAttributeParser()
{
}

void CDefaultAttributeParser::Parse(const std::wstring& attrName, ChangeUtils::ObjectChange::AttributeChange* attrChange, const ChangeUtils::ObjectChange&)
{
	for (auto& attr : attrChange->newValues)
	{
		const_cast<std::wstring&>(attr.parsedValue) = attr.rawValue;
	}

	for (auto& attr : attrChange->oldValues)
	{
		const_cast<std::wstring&>(attr.parsedValue) = attr.rawValue;
	}

	if (attrChange->newValues.empty())
	{
		// cleared
		attrChange->hrt = L"Attribute '" + attrName + L"' was cleared";
	}
	else if (attrChange->oldValues.empty())
	{
		//set
		attrChange->hrt = L"Attribute '" + attrName + L"' was set to '" + attrChange->newValues.begin()->parsedValue + L"'";
	}
	else if (!attrChange->newValues.empty() && !attrChange->oldValues.empty())
	{
		//change
		attrChange->hrt = L"Attribute '" + attrName + L"' was changed from '" + attrChange->oldValues.begin()->parsedValue + L"' to '"
			+ attrChange->newValues.rbegin()->parsedValue + L"'";
	}
	else
	{
		attrChange->hrt = L"HRT composing error";
	}
}