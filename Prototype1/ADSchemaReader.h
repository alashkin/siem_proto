#pragma once

#include <set>
#include <string>
#include "ad_utils.h"

class CADSchemaReader
{
public:

	void Read(ADUtils::SchemaInfo* schemaInfo);

	CADSchemaReader(const std::wstring& dc, const std::wstring& domainfqdn,
					const std::wstring& user, const std::wstring& password);
	virtual ~CADSchemaReader();

private:
	const std::wstring m_dc;
	const std::wstring m_domainfqdn;
	const std::wstring m_user; 
	const std::wstring m_password;

};

