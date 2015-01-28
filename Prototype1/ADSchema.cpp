#include "stdafx.h"
#include "ADSchema.h"


CADSchema::CADSchema(const ADUtils::SchemaInfo& schemaInfo) : m_schemaInfo(schemaInfo)
{

}


CADSchema::~CADSchema()
{
}

bool CADSchema::IsAttributeMultivalied(const std::wstring& attribute_name)
{
	return (m_schemaInfo.multivalued_attributes.find(attribute_name) !=
											m_schemaInfo.multivalued_attributes.end());
}
