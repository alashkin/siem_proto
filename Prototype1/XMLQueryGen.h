#pragma once
#include "Log.h"
#include <pugixml.hpp>
#include "Config.h"
#include <string>
#include "XMLQueryGenBase.h"

class CXMLQueryGen : public IXMLQueryGen
{
public:
	void Generate(const Config::events_to_collect_t& events, const Config::EventsToSuppress& suppress, unsigned int maxEvents);
	virtual const std::wstring& GetQuery() override;

	CXMLQueryGen();
	virtual ~CXMLQueryGen();

private:
	std::wstring m_query;

	Config::events_t::iterator AddSelect(pugi::xml_node& xmlQueryPath,
		const std::wstring& path,
		const Config::events_t& events,
		Config::events_t::iterator& itBegin,
		unsigned int max);

	void AddSuppress(const Config::EventsToSuppress& suppress, pugi::xml_node& xmlQuery, const std::wstring& path);

};

