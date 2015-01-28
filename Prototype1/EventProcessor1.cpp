#include "stdafx.h"
#include "EventProcessor1.h"
#include "Log.h"
#include <map>
#include "EvtConvert.h"
#include <set>

void CEventProcessor1::AddEvent(Event& evt)
{
	m_events.push_back(evt);
}

struct DisplayChange
{
	DisplayChange() : RecordID(0xFFFFFFFFFFFFFFFF), timeCreated(0xFFFFFFFFFFFFFFFF) {};
	UINT64 RecordID;
	ULONGLONG timeCreated;
	std::wstring changeDescription;
};

inline bool operator < (const DisplayChange& dc1, const DisplayChange& dc2)
{
	if (dc1.timeCreated != dc2.timeCreated)
	{
		return dc1.timeCreated < dc2.timeCreated;
	}
	else
	{
		return dc1.RecordID < dc2.RecordID;
	}
}

void CEventProcessor1::ProcessEvents()
{
	_LOG(debug) << L"Events to process: " << m_events.size();

	std::map<std::wstring, std::vector<Event> > events_by_corrID;

	// раскладываем по corrID

	for (auto& evt : m_events)
	{
		auto corrID = evt.eventData.find(L"OpCorrelationID");
		if (corrID == evt.eventData.end())
		{
			_LOG(debug) << "No correlation id";
		}
		else
		{
			events_by_corrID[corrID->second].push_back(evt);
		}
	}


	std::set<DisplayChange> changesToDisplay;

	// расфасовываем чейнджи
	for (auto& evts : events_by_corrID)
	{
		_LOG(debug) << "Correlation id: " << evts.first;

		// определяем тип
		enum OPTYPE { opNone, opCreated, opModified, opDeleted };
		OPTYPE optype = opModified;

		for (auto& evt : evts.second)
		{
			if (evt.system.EvtSystemEventID == 5137)
			{
				// создание
				optype = opCreated;
				break;
			}

			if (evt.system.EvtSystemEventID == 5141)
			{
				// удаление
				optype = opDeleted;
				break;
			}
		}

		std::map<std::wstring, std::wstring> props_to_display;

		if (optype == opCreated )
		{
			DisplayChange dc;
			dc.changeDescription += L"### Object created ###\r\n";

			auto& evt = *evts.second.begin();
			dc.changeDescription += L"$WHO: " + evt.eventData[L"SubjectDomainName"] + L"\\" + evt.eventData[L"SubjectUserName"] + L"\r\n";
			dc.changeDescription += L"$WHEN: " + boost::lexical_cast<std::wstring>(evt.system.EvtSystemTimeCreated) +L"\r\n";
			dc.changeDescription += L"$WHAT: " + evt.eventData[L"ObjectDN"] + L" (" + evt.eventData[L"ObjectClass"] + L")\r\n";

			dc.changeDescription += L"$ATTRIBUTES: \r\n";
			for (auto& evt : evts.second)
			{
				if (dc.RecordID > evt.system.EvtSystemEventRecordId)
				{
					dc.RecordID = evt.system.EvtSystemEventRecordId;
				}

				if (dc.timeCreated > *(ULONGLONG*)&evt.system.EvtSystemTimeCreated)
				{
					dc.timeCreated = *(ULONGLONG*)&evt.system.EvtSystemTimeCreated;
				}

				if (evt.eventData.find(L"AttributeLDAPDisplayName") != evt.eventData.end())
				{
					std::wstring value = evt.eventData[L"AttributeValue"];
					if (value.empty())
					{
						value = L"<empty>";
					}
					dc.changeDescription += evt.eventData[L"AttributeLDAPDisplayName"] + L" : " + value + L"\r\n";
				}
			}

			changesToDisplay.insert(dc);
		}
		else if (optype == opModified)
		{
			DisplayChange dc;
			dc.changeDescription += L"### Object modified ###\r\n";

			auto& evt = *evts.second.begin();
			dc.changeDescription += L"$WHO: " + evt.eventData[L"SubjectDomainName"] + L"\\" + evt.eventData[L"SubjectUserName"] + L"\r\n";
			dc.changeDescription += L"$WHEN: " + boost::lexical_cast<std::wstring>(evt.system.EvtSystemTimeCreated) + L"\r\n";
			dc.changeDescription += L"$WHAT: " + evt.eventData[L"ObjectDN"] + L" (" + evt.eventData[L"ObjectClass"] + L")\r\n";

			dc.changeDescription += L"$ATTRIBUTES: \r\n";

			typedef std::pair<std::wstring, std::wstring> new_old_value;
			std::map<std::wstring, new_old_value> attributes_changes;

			for (auto& evt : evts.second)
			{
				if (dc.RecordID > evt.system.EvtSystemEventRecordId)
				{
					dc.RecordID = evt.system.EvtSystemEventRecordId;
				}

				if (dc.timeCreated > *(ULONGLONG*)&evt.system.EvtSystemTimeCreated)
				{
					dc.timeCreated = *(ULONGLONG*)&evt.system.EvtSystemTimeCreated;
				}

				//Attribute -> (old, new);
				if (evt.eventData.find(L"AttributeLDAPDisplayName") != evt.eventData.end())
				{
					if (evt.eventData[L"OperationType"] == L"%%14674") // added (new)
					{
						if (evt.eventData[L"ObjectClass"] == L"group"
							&& evt.eventData[L"AttributeLDAPDisplayName"] == L"member")
						{
							std::wstring value = evt.eventData[L"AttributeValue"];
							value = L"\r\n" + value;

							attributes_changes[evt.eventData[L"AttributeLDAPDisplayName"]].first += value;
							attributes_changes[evt.eventData[L"AttributeLDAPDisplayName"]].second = L"added";
						}
						else
						{
							std::wstring value = evt.eventData[L"AttributeValue"];
							if (value.empty())
							{
								value = L"<empty>";
							}

							if (!attributes_changes[evt.eventData[L"AttributeLDAPDisplayName"]].first.empty())
							{
								attributes_changes[evt.eventData[L"AttributeLDAPDisplayName"]].first += L"\r\n";
							}

							attributes_changes[evt.eventData[L"AttributeLDAPDisplayName"]].first += value;
						}
					}
					else // deleted (old)
					{
						if (evt.eventData[L"ObjectClass"] == L"group"
							&& evt.eventData[L"AttributeLDAPDisplayName"] == L"member")
						{
							std::wstring value = evt.eventData[L"AttributeValue"];
							value = L"\r\n" + value;

							attributes_changes[evt.eventData[L"AttributeLDAPDisplayName"]].first += value;
							attributes_changes[evt.eventData[L"AttributeLDAPDisplayName"]].second = L"deleted";
						}
						else
						{
							std::wstring value = evt.eventData[L"AttributeValue"];
							if (value.empty())
							{
								value = L"<empty>";
							}

							if (!attributes_changes[evt.eventData[L"AttributeLDAPDisplayName"]].second.empty())
							{
								attributes_changes[evt.eventData[L"AttributeLDAPDisplayName"]].second += L"\r\n";
							}

							attributes_changes[evt.eventData[L"AttributeLDAPDisplayName"]].second += value;
						}
					}
				}
			}

			for (auto& attr : attributes_changes)
			{
				std::wstring oldval = attr.second.second.empty() ? L"<empty>" : attr.second.second;
				std::wstring newval = attr.second.first.empty() ? L"<empty>" : attr.second.first;
				dc.changeDescription += attr.first + L" : " + oldval + L" -> " + newval + L"\r\n";
			}

			changesToDisplay.insert(dc);
		}
		else if (optype = opDeleted)
		{
			DisplayChange dc;
			dc.changeDescription += L"### Object deleted ###\r\n";

			auto& evt = *evts.second.begin();
			dc.changeDescription += L"$WHO: " + evt.eventData[L"SubjectDomainName"] + L"\\" + evt.eventData[L"SubjectUserName"] + L"\r\n";
			dc.changeDescription += L"$WHEN: " + boost::lexical_cast<std::wstring>(evt.system.EvtSystemTimeCreated) + L"\r\n";
			dc.changeDescription += L"$WHAT: " + evt.eventData[L"ObjectDN"] + L" (" + evt.eventData[L"ObjectClass"] + L")\r\n";
			dc.RecordID = evt.system.EvtSystemEventRecordId;
			dc.timeCreated = *(ULONGLONG*)&evt.system.EvtSystemTimeCreated;

			changesToDisplay.insert(dc);
		}
	}

	for (auto& change : changesToDisplay)
	{
		_LOG(debug) << change.changeDescription;
	}

	m_events.clear();
}


CEventProcessor1::CEventProcessor1()
{
}


CEventProcessor1::~CEventProcessor1()
{
}
