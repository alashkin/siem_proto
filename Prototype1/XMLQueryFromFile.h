#pragma once
#include "XMLQueryGenBase.h"
#include <string>

class CXMLQueryFromFile : public IXMLQueryGen
{
private:
	std::wstring m_query;

public:
	void Load(const std::wstring& fileName);
	virtual const std::wstring& GetQuery() override;


	CXMLQueryFromFile();
	virtual ~CXMLQueryFromFile();
};

