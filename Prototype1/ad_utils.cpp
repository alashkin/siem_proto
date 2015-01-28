#include "stdafx.h"
#include "ad_utils.h"

#include <boost/algorithm/string.hpp>
#include <stdexcept>
#include <boost/algorithm/string/erase.hpp>
#include "unicode.h"

std::wstring ADUtils::ExtractNameFromDN(const std::wstring& dn)
{
	std::wstring tmp = boost::to_upper_copy(dn);

	std::wstring::size_type delim_pos;

	if ((delim_pos = tmp.find(L"CN=", 3)) == std::wstring::npos)
	{
		if ((delim_pos = tmp.find(L"OU=", 3)) == std::wstring::npos)
		{
			if ((delim_pos = tmp.find(L"DC=", 3)) == std::wstring::npos)
			{
				throw std::invalid_argument("Invalid DN: " + boost::lexical_cast<ansi::string>(dn));
			}
		}
	}

	tmp = dn.substr(3, delim_pos - 3 - 1);

	boost::erase_all(tmp, LR"(\)");

	return tmp;
}
