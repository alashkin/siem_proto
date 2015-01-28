#pragma once

#include "ad_utils.h"

class CADSchemaSerializer
{
public:
	struct Params
	{
		std::wstring fileName;
	};

	CADSchemaSerializer(const Params& params);
	virtual ~CADSchemaSerializer();

	void Read(ADUtils::SchemaInfo* schema);
	void Write(const ADUtils::SchemaInfo& schema);

private:
	Params m_params;

};

