#include "stdafx.h"
#include "GroupTypeParser.h"
#include <Windows.h>
#include <boost/lexical_cast.hpp>
#include <iads.h>
#include "AttrParserUtils.h"

CGroupTypeParser::CGroupTypeParser()
{
}


CGroupTypeParser::~CGroupTypeParser()
{
}

void CGroupTypeParser::Parse(const std::wstring& , ChangeUtils::ObjectChange::AttributeChange* attrChange, const ChangeUtils::ObjectChange&)
{
	ChangeUtils::ObjectChange::attribute_values_t::iterator oldValue = attrChange->oldValues.begin();
	ChangeUtils::ObjectChange::attribute_values_t::reverse_iterator newValue = attrChange->newValues.rbegin();

	if (oldValue != attrChange->oldValues.end())
	{
		const_cast<std::wstring&>(oldValue->parsedValue) = ParseOneValue(oldValue->rawValue);
	}

	if (newValue != attrChange->newValues.rend())
	{
		const_cast<std::wstring&>(newValue->parsedValue) = ParseOneValue(newValue->rawValue);
	}

	
	if (oldValue == attrChange->oldValues.end() && newValue != attrChange->newValues.rend())
	{
		// oldValue = empty, newValue !- empty => attribute was setted
		attrChange->hrt = L"Group type was set to '" + newValue->parsedValue + L"'";
	}
	else if (oldValue != attrChange->oldValues.end() && newValue != attrChange->newValues.rend())
	{
		//	oldValue != empty, newValue !- empty => attribute was changed
		attrChange->hrt = L"Group type was changed from '" + oldValue->parsedValue + L"' to '" + newValue->parsedValue + L"'";
	}
	else
	{
		attrChange->hrt = L"HRT composing error";
	}
}

std::wstring CGroupTypeParser::ParseOneValue(const std::wstring& attrValue)
{
	static struct 
	{
		ADS_GROUP_TYPE_ENUM value;
		std::wstring text;
	} groupTypesText[] =
	{
		{
			ADS_GROUP_TYPE_GLOBAL_GROUP,
			L"Global"
		},
		{
			ADS_GROUP_TYPE_DOMAIN_LOCAL_GROUP,
			L"Local"
		},
		{
			ADS_GROUP_TYPE_LOCAL_GROUP,
			L"Local",
		},
		{
			ADS_GROUP_TYPE_UNIVERSAL_GROUP,
			L"Universal"
		},
		{
			ADS_GROUP_TYPE_SECURITY_ENABLED,
			L"Security"
		}
	};


	std::wstring retVal;

	UINT32 groupType = boost::lexical_cast<UINT32>(attrValue);

	retVal = AttrParserUtils::ConvertBitMask(groupType, groupTypesText);

	if ((groupType & ADS_GROUP_TYPE_SECURITY_ENABLED) == 0)
	{
		retVal += (retVal.empty() ? L"" : L" ") + std::wstring(L"Distribution");
	}


	return retVal;
}
