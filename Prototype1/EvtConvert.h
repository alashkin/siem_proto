#pragma once
#pragma once

#include <boost/lexical_cast.hpp>
#include <Windows.h>
#include <string>
#include <atlsecurity.h>


namespace boost
{
	namespace details
	{
		std::wstring _ConvertFileTimeToString(const FILETIME& ft);
		std::wstring _ConvertGUIDToString(const GUID& guid);
		std::wstring _ConvertSIDToString(const ATL::CSid& sid);
	}

	template <>
		inline std::wstring lexical_cast(const ATL::CSid& arg)
		{
			return details::_ConvertSIDToString(arg);
		}

	template <>
		inline std::wstring lexical_cast(const FILETIME& arg)
		{
			return details::_ConvertFileTimeToString(arg);
		}

	template <>
		inline std::wstring lexical_cast(const GUID& arg)
		{
			return details::_ConvertGUIDToString(arg);
		}

}

