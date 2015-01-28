#include "stdafx.h"
#include "AttributeValuesParser.h"
#include <string>

CAttributeValuesParser::CAttributeValuesParser(const Params& params) : m_params(params)
{
}


CAttributeValuesParser::~CAttributeValuesParser()
{
}

void CAttributeValuesParser::Parse(ChangeUtils::ObjectChange* objectChange)
{
	std::wstring& objectClass = objectChange->changeInfo.data[ChangeUtils::Consts::WHAT_Class];
	auto& classParsers = m_params.parsers->find(objectClass);
	
	for(auto& attr : objectChange->changedAttributes )
	{
		std::shared_ptr<CAttributeValueParserBase> attributeParser = m_params.defaultParser;

		if (classParsers != m_params.parsers->end())
		{
			auto& attrParser = classParsers->second.find(attr.first);

			if (attrParser != classParsers->second.end())
			{
				attributeParser = attrParser->second;
			}
		}

		attributeParser->Parse(attr.first, &attr.second, *objectChange);
	}
}