#include "stdafx.h"
#include "EventWithFlatEventDataParser.h"
#include "Log.h"
#include <iostream>
#include <stdexcept>
#include <sstream>

void CEventWithFlatEventDataParser::ProceedNode(pugi::xml_node& node, EventUtils::ParsedEvent1::eventdata_t* data)
{
	for (auto& elem : node.children())
	{
		auto& name = elem.attribute(L"Name");
		if (!name)
		{
			std::wstringstream ss;
			elem.print(ss);
			data->insert(std::make_pair(L"XML", ss.str()));
		}
		else
		{
			auto& value = elem.text();
			data->insert(std::make_pair(name.as_string(), value.as_string()));
		}
	}
}

std::shared_ptr<EventUtils::ParsedEvent1::eventdata_t> CEventWithFlatEventDataParser::Parse(const EventUtils::RawEvent1& evt)
{
	return Parse(evt.eventXML->c_str(), evt.eventXML->size() * sizeof(wchar_t));
}


std::shared_ptr<EventUtils::ParsedEvent1::eventdata_t> CEventWithFlatEventDataParser::Parse(const wchar_t* evtxml, size_t size)
{
	_LOG(debug) << L"Start parsing XML event";
	auto eventData = std::make_shared<EventUtils::ParsedEvent1::eventdata_t>();

	pugi::xml_document doc;
	pugi::xml_parse_result res = doc.load_buffer(evtxml, size);

	pugi::xml_node root = *doc.children().begin();

	if (root)
	{
		pugi::xml_node ed = root.child(L"EventData");
		if (ed)
		{
			ProceedNode(ed, eventData.get());
		}
		else
		{
			_LOG(warning) << L"No EventData in event";
		}

	}
	else
	{
		// õ.ç.
		_LOG(error) << L"Unknown event XML format: " << evtxml;
	}

	_LOG(debug) << L"End parsing XML event";

	return eventData;
}
