#pragma once
#include <string>

class IXMLQueryGen
{
public:
	virtual const std::wstring& GetQuery() = 0;
	virtual ~IXMLQueryGen() {};
};