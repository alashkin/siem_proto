#pragma once

#include "Config.h"
#include <memory>
#include <string>

class CBkmConfig 
{
private:
	class CBookmarkFileStorage : public Config::IBookmarkStorage
	{
	private:
		std::wstring m_fileName;

	public:
		CBookmarkFileStorage(const std::wstring& fileName);
		virtual std::wstring Load() override;
		virtual void Save(const std::wstring& bkmText) override;
	};

	class CBookmarkFakeStorage : public Config::IBookmarkStorage
	{
		virtual std::wstring Load() override;
		virtual void Save(const std::wstring& bkmText) override;
	};

	std::wstring m_bkmPath;

public:
	std::shared_ptr<Config::IBookmarkStorage> GetBkmStorage(const std::wstring& bkmName);

	CBkmConfig(const std::wstring& bkmPath);
	virtual ~CBkmConfig();
};

