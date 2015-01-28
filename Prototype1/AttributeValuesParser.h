#pragma once

#include <map>
#include <string>
#include "AttributeValueParserBase.h"
#include <memory>
#include "ChangeUtils.h"
#include "utils.h"

class CAttributeValuesParser
{
public:
	typedef std::map<std::wstring, std::shared_ptr<CAttributeValueParserBase>, utils::CaseInsensitiveLess> attrname_to_parser_t;
	typedef std::map<std::wstring, attrname_to_parser_t, utils::CaseInsensitiveLess> parsers_map_t;

	struct Params
	{
		std::shared_ptr<parsers_map_t> parsers;
		std::shared_ptr<CAttributeValueParserBase> defaultParser;
	};

	CAttributeValuesParser(const Params& params);
	virtual ~CAttributeValuesParser();

	void Parse(ChangeUtils::ObjectChange* objectChange);

private:
	Params m_params;

};

