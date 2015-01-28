#pragma once

#include <memory>
#include <pugixml.hpp>
#include "EventUtils.h"

class CClearEventLogEventParser
{
public:

	static std::shared_ptr<EventUtils::ParsedEvent1::eventdata_t> Parse(EventUtils::RawEvent1& evt)
	{
		auto userData = std::make_shared<EventUtils::ParsedEvent1::eventdata_t>();

		pugi::xml_document doc;
		pugi::xml_parse_result res = doc.load_buffer(evt.eventXML->c_str(), evt.eventXML->size()*sizeof(wchar_t));

		pugi::xml_node root = *doc.children().begin();

		if (root)
		{
			pugi::xml_node ud = root.child(L"UserData");
			if (ud)
			{
				pugi::xml_node lfc = ud.child(L"LogFileCleared");

				if (lfc)
				{
					userData->insert(std::make_pair(EventUtils::Consts::EventData::WHO_Name,
						lfc.child(L"SubjectUserName").text().as_string()));

					userData->insert(std::make_pair(EventUtils::Consts::EventData::WHO_Sid,
						lfc.child(L"SubjectUserSid").text().as_string()));

					userData->insert(std::make_pair(EventUtils::Consts::EventData::WHO_DomainNetbios,
						lfc.child(L"SubjectDomainName").text().as_string()));

				}
			}
		}

		return userData;
	}
};
