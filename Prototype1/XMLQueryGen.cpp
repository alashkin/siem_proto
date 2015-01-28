#include "stdafx.h"
#include "XMLQueryGen.h"
#include "unicode.h"
#include <boost/lexical_cast.hpp>
#include <string>
#include <vector>
#include <map>

Config::events_t::iterator CXMLQueryGen::AddSelect(pugi::xml_node& xmlQueryPath,
	const std::wstring& path,
	const Config::events_t& events,
	Config::events_t::iterator& itBegin,
	unsigned int max )
{
	Config::events_t::iterator it = itBegin;
	unsigned int processed = 0;

	std::wstring strSelect;

	if (events.size() != 0)
	{
		strSelect = L"Event/System[";

		for (it = itBegin; it != events.end(); it++)
		{
			strSelect += L"EventID=" + boost::lexical_cast<std::wstring>(*it);

			if (++processed == max)
			{
				break;
			}

			if (it != --events.end())
			{
				strSelect += L" or ";
			}
		}

		strSelect += L"]";
	}
	else
	{
		strSelect = L"*";
	}

	pugi::xml_node xmlSelect = xmlQueryPath.append_child(L"Select");
	xmlSelect.text().set(strSelect.c_str());

	
	return it != events.end() ? ++it : events.end();
}

void CXMLQueryGen::AddSuppress(const Config::EventsToSuppress& suppress, pugi::xml_node& xmlQuery, const std::wstring& path)
{
	for (auto& and_map : suppress.event_data_51xx_filter)
	{ //or
		std::wstring strSuppress = L"Event/EventData[";

		int processed = 0;

		for (auto& and : and_map)
		{ // and
			strSuppress += L"(Data[@Name='" + and.first + L"'] and Data='" + and.second + L"')";
			
			if (++processed != and_map.size())
			{
				strSuppress += L" and ";
			}
		}

		strSuppress += L"]";
		pugi::xml_node xmlSupress = xmlQuery.append_child(L"Suppress");
		xmlSupress.text().set(strSuppress.c_str());
	}
}

void CXMLQueryGen::Generate(const Config::events_to_collect_t& events,const  Config::EventsToSuppress& suppress, unsigned int maxEvents)
{
	_LOG(debug) << "Generating query from IDs";

	pugi::xml_document doc;
	pugi::xml_node xmlQueryList = doc.append_child(L"QueryList");


	for (auto i : events)
	{
		pugi::xml_node xmlQueryPath = xmlQueryList.append_child(L"Query");
		xmlQueryPath.append_attribute(L"Path") = i.first.c_str();

		Config::events_t::iterator evtIter = i.second.begin();
		while ((evtIter = AddSelect(xmlQueryPath, i.first, i.second, evtIter, maxEvents)) != i.second.end()) 
		{
			//....
		}

		AddSuppress(suppress, xmlQueryPath, i.first);
	}


	std::wstringstream ss;
	xmlQueryList.print(ss);
	m_query= ss.str();

	_LOG(debug) << "Query XML Created:\r\n" << m_query;
}

const std::wstring& CXMLQueryGen::GetQuery()
{
	return m_query;
}

CXMLQueryGen::CXMLQueryGen()
{
}


CXMLQueryGen::~CXMLQueryGen()
{
}
