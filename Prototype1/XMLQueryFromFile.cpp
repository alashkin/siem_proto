#include "stdafx.h"
#include "XMLQueryFromFile.h"
#include "unicode.h"
#include "Log.h"
#include "utils.h"
#include <vector>

void CXMLQueryFromFile::Load(const std::wstring& fileName)
{
	_LOG(debug) << "Loading query from " << fileName;

	CHandle hFile(::CreateFile(fileName.c_str(), GENERIC_READ, 0, 0, OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL, 0));
	if (hFile == INVALID_HANDLE_VALUE)
	{
		throw std::runtime_error(utils::FormatLastErrorA("CreateFile"));
	}

	DWORD dwSize;
	if ((dwSize = ::GetFileSize(hFile, 0)) == INVALID_FILE_SIZE)
	{
		throw std::runtime_error(utils::FormatLastErrorA("GetFileSize"));
	}

	std::vector<char> buf(dwSize + 1, 0);

	DWORD dwReaden;
	if (!::ReadFile(hFile, buf.data(), dwSize, &dwReaden, 0))
	{
		throw std::runtime_error(utils::FormatLastErrorA("ReadFile"));
	}

	m_query = boost::lexical_cast<std::wstring>(ansi::string(buf.data()));

	_LOG(debug) << "Loaded query:" << m_query;
}

const std::wstring& CXMLQueryFromFile::GetQuery()
{
	return m_query;
}

CXMLQueryFromFile::CXMLQueryFromFile()
{
}


CXMLQueryFromFile::~CXMLQueryFromFile()
{
}
