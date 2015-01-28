#include "stdafx.h" 


#include "EvtConvert.h"
#include <string>
#include "unicode.h"
#include <boost/format.hpp>
#include <objbase.h>
#include <sddl.h>
#include <memory>


std::wstring boost::details::_ConvertFileTimeToString(const FILETIME& ft)
{
	SYSTEMTIME st;
	::FileTimeToSystemTime(&ft, &st);

	boost::format fmt = boost::format("%1%.%2%.%3% %4%:%5%:%6%.%7%") % st.wYear % st.wMonth % st.wDay % st.wHour
		% st.wMinute % st.wSecond %st.wMilliseconds;

	return boost::lexical_cast<std::wstring>(ansi::string(fmt.str()));
}

std::wstring boost::details::_ConvertGUIDToString(const GUID& guid)
{
	std::wstring res;

	LPOLESTR pszGuid;
	::StringFromCLSID(guid, &pszGuid);
	res = pszGuid;
	::CoTaskMemFree(pszGuid);

	return res;
}

std::wstring boost::details::_ConvertSIDToString(const ATL::CSid& sid)
{
	std::wstring res;

	if (sid.IsValid())
	{
		LPWSTR pszSid;
		::ConvertSidToStringSid((const PSID)sid.GetPSID(), &pszSid);
		res = pszSid;
		::LocalFree(pszSid);
	}

	return res;
}
