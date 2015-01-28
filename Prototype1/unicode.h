#pragma once

#include <string>
#include <atlbase.h>
#include <boost/lexical_cast.hpp>
#include <stdexcept>

namespace ansi
{
	typedef std::string string;
}

namespace boost
{
	template <>
		inline ansi::string lexical_cast(const std::wstring& arg)
		{
			return LPSTR(ATL::CW2A(arg.c_str()));
		}

	template <>
		inline std::wstring lexical_cast(const ansi::string& arg)
		{
			return LPWSTR( ATL::CA2W(arg.c_str()));
		}

	template <>
		inline std::wstring lexical_cast(const std::exception& e)
		{
			return LPWSTR(ATL::CA2W(e.what()));
		}

}
