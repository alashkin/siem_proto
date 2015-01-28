#pragma once

#include <boost/multi_index/detail/scope_guard.hpp>
#include <boost/lexical_cast.hpp>
#include "unicode.h"
#include <stdexcept>
#include <Stringapiset.h>
#include <vector>
#include <Windows.h>
#include <Strsafe.h>
#include <boost/algorithm/string.hpp>
#include <Iads.h>
#include <AdsHlp.h>
#include <stdio.h>

namespace boost
{
	namespace utils = boost::multi_index::detail;
}

namespace utils
{
	inline ansi::string FormatLastErrorA(const char* pszFuncName)
	{
		ansi::string retVal;
		retVal = pszFuncName ? pszFuncName : "Win32 error: ";
		DWORD dwErr = ::GetLastError();
		retVal += " #" + boost::lexical_cast<ansi::string>(dwErr);

		char buf[1024 * 32] = { 0 };

		DWORD dwLen;
		if (dwLen = ::FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM,
			0, dwErr, 0, buf, sizeof(buf), 0))
		{
			if (dwLen > 2 && (buf[dwLen - 2] == '\r' || buf[dwLen - 2] == '\n'))
			{
				buf[dwLen - 2] = 0;
			}

			retVal += " ("; retVal += buf; retVal += ")";
		}


		return retVal;
	}


	inline void ConvertUINT64FTtoString(const UINT64& time, std::wstring* res)
	{
		SYSTEMTIME st;
		::FileTimeToSystemTime((FILETIME*)&time, &st);
		wchar_t buf[100];
		::StringCbPrintfW(buf, sizeof(buf), L"%04hu-%02hu-%02hu %02hu:%02hu:%02hu.%03hu", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wMilliseconds);

		*res = buf;
	}

	struct CaseInsensitiveLess : public std::binary_function<std::wstring, std::wstring, bool>
	{
		bool operator()(const std::wstring &s1, const std::wstring &s2) const
		{
			return _wcsicmp(s1.c_str(), s2.c_str()) < 0;
		}
	};

	inline ansi::string FormatADsResult(HRESULT hr)
	{
		DWORD dwLastError = 0;
		WCHAR szErrorBuf[MAX_PATH] = { 0 };
		WCHAR szNameBuf[MAX_PATH] = { 0 };

		::ADsGetLastError(&dwLastError,
			szErrorBuf, sizeof(szErrorBuf) / sizeof(szErrorBuf[0]),
			szNameBuf, sizeof(szNameBuf) / sizeof(szNameBuf[0]));

		char szErrorDescription[32000];
		sprintf(szErrorDescription, "Error: RetCode = 0x%X, ExtendedCode = 0x%X, Description: %S, Provider: %S",
			hr, dwLastError, szErrorBuf, szNameBuf);

		return szErrorDescription;

	}

	inline void CheckADsResult(HRESULT hr)
	{
		if (FAILED(hr))
		{
			throw std::runtime_error(FormatADsResult(hr));
		}
	}
}
//	ex: boost::utils::scope_guard sg = boost::utils::make_guard( [&](){ myfree(p); } );
