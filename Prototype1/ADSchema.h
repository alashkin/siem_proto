#pragma once

#include <string>
#include "ad_utils.h"

class CADSchema
{
public:
	bool IsAttributeMultivalied(const std::wstring& attribute_name);

	CADSchema(const ADUtils::SchemaInfo& schemaInfo);
	virtual ~CADSchema();

private:
	ADUtils::SchemaInfo m_schemaInfo;

};

