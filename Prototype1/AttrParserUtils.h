#pragma once

#include <string>

namespace AttrParserUtils
{
	template <class TABLE> std::wstring ConvertBitMask(UINT32 bitMask, const TABLE& table)
	{
		std::wstring retVal;

		for (auto& unmask : table)
		{
			if ((bitMask & unmask.value) != 0)
			{
				retVal += (retVal.empty() ? L"" : L" ") + unmask.text;

				bitMask -= unmask.value;
			}
		}

		if (bitMask != 0)
		{
			retVal += (retVal.empty() ? L"" : L" ") 
						+ std::wstring(L"[Unknown value: ") + boost::lexical_cast<std::wstring>(bitMask)+L"]";
		}

		return retVal;
	}
}