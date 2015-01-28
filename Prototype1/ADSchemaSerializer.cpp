#include "stdafx.h"
#include "ADSchemaSerializer.h"
#include <fstream>

CADSchemaSerializer::CADSchemaSerializer(const Params& params) : m_params(params)
{

}

CADSchemaSerializer::~CADSchemaSerializer()
{

}

void CADSchemaSerializer::Read(ADUtils::SchemaInfo* schema)
{
	std::wifstream file;
	file.open(m_params.fileName);

	std::wstring attribute;
	while (file >> attribute)
	{
		schema->multivalued_attributes.insert(attribute);
	}
}

void CADSchemaSerializer::Write(const ADUtils::SchemaInfo& schema)
{
	std::wofstream file;
	file.open(m_params.fileName);

	for (auto& attribute : schema.multivalued_attributes)
	{
		file << attribute;
	}
}
