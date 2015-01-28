#pragma once

#include <map>
#include <string>
#include <pugixml.hpp>
#include "EventUtils.h"
#include <memory>

class CEventWithFlatEventDataParser
{
public:
	static std::shared_ptr<EventUtils::ParsedEvent1::eventdata_t> Parse(const EventUtils::RawEvent1& evt);

private:
	static void ProceedNode(pugi::xml_node& node, EventUtils::ParsedEvent1::eventdata_t* data);
	static std::shared_ptr<EventUtils::ParsedEvent1::eventdata_t> Parse(const wchar_t* evtxml, size_t size);

};

