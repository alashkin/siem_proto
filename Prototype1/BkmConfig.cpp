#include "stdafx.h"
#include "BkmConfig.h"
#include "unicode.h"
#include <atlbase.h>
#include <Windows.h>
#include <stdexcept>
#include "utils.h"
#include <vector>		
#include <boost/algorithm/string.hpp>
#include "log.h"

std::wstring CBkmConfig::CBookmarkFakeStorage::Load()
{
	return L"";
}

void CBkmConfig::CBookmarkFakeStorage::Save(const std::wstring&)
{

}

CBkmConfig::CBookmarkFileStorage::CBookmarkFileStorage(const std::wstring& fileName)
{
	m_fileName = fileName;
}

std::wstring CBkmConfig::CBookmarkFileStorage::Load()
{
	CHandle hFile( ::CreateFile(m_fileName.c_str(), GENERIC_READ, 0, 0, OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL, 0) );
	if (hFile == INVALID_HANDLE_VALUE)
	{
		if (::GetLastError() != ERROR_FILE_NOT_FOUND)
		{
			throw std::runtime_error(utils::FormatLastErrorA("CreateFile"));
		}
		else
		{
			return L"";
		}
	}

	DWORD dwSize;
	if ((dwSize = ::GetFileSize(hFile, 0)) == INVALID_FILE_SIZE)
	{
		throw std::runtime_error(utils::FormatLastErrorA("GetFileSize"));
	}

	std::vector<BYTE> buf(dwSize + 2, 0);

	DWORD dwReaden;
	if (!::ReadFile(hFile, buf.data(), dwSize, &dwReaden, 0))
	{
		throw std::runtime_error(utils::FormatLastErrorA("ReadFile"));
	}

	return (wchar_t*)buf.data();
}

void CBkmConfig::CBookmarkFileStorage::Save(const std::wstring& bkmText)
{
	CHandle hFile(::CreateFile(m_fileName.c_str(), GENERIC_WRITE, 0, 0, CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL, 0));
	if (hFile == INVALID_HANDLE_VALUE)
	{
		throw std::runtime_error(utils::FormatLastErrorA("CreateFile"));
	}

	DWORD dwWritten;
	if (!::WriteFile(hFile, bkmText.data(), bkmText.size() * sizeof(bkmText[0]),
		&dwWritten, 0))
	{
		throw std::runtime_error(utils::FormatLastErrorA("WriteFile"));
	}
}

std::shared_ptr<Config::IBookmarkStorage> CBkmConfig::GetBkmStorage(const std::wstring& bkmName)
{
	if (!boost::iequals(m_bkmPath, "fake"))
	{
		std::shared_ptr<CBookmarkFileStorage> bkmStorage = 
			std::make_shared<CBookmarkFileStorage>(m_bkmPath + L"\\" + bkmName + L"_bookmark.evtbkm");

		return std::static_pointer_cast<Config::IBookmarkStorage>(bkmStorage);
	}
	else
	{
		_LOG(debug) << L"Using FakeBookmarkStorage";
		return std::static_pointer_cast<Config::IBookmarkStorage>(std::make_shared<CBookmarkFakeStorage>());
	}
}


CBkmConfig::CBkmConfig(const std::wstring& bkmPath)
{
	m_bkmPath = bkmPath;
}

CBkmConfig::~CBkmConfig()
{
}
