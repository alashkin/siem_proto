#pragma once

#include <string>
#include <set>

namespace ADUtils
{
	std::wstring ExtractNameFromDN(const std::wstring& dn);

	struct SchemaInfo
	{
		typedef std::set<std::wstring> multivalued_attributes_t;
		multivalued_attributes_t multivalued_attributes;
	};
}