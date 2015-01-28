#pragma once
#include "Config.h"
#include <json_spirit/json_spirit.h>
#include <memory>

class CJSONConfig 
{
public:
	enum QueryType { qtEvents, qtFile };

private:

	void LoadAll(const json_spirit::wmValue& value);
	
	void LoadLogsAndEvents(const json_spirit::wmObject& objRoot);
	void LoadTargets(const json_spirit::wmObject& objRoot);
	void LoadSettings(const json_spirit::wmObject& objRoot);

	Config::events_to_collect_t m_events;
	Config::targets_t m_targets;
	Config::Settings m_settings;
	std::wstring m_queryFile;
	unsigned int m_maxIDinQuery;
	QueryType m_queryType;
	Config::EventsToSuppress m_eventsToSuppress;

public:

	void Init(const std::wstring& filename); 

	const Config::events_to_collect_t& GetEventsToCollect() { return m_events; };
	const Config::targets_t& GetTargets() { return m_targets; };
	const Config::Settings& GetSettings() { return m_settings; };
	const QueryType GetQueryType() { return m_queryType; };
	const std::wstring& GetQueryFile() { return m_queryFile; };
	unsigned int GetMaxIDinQuery() { return m_maxIDinQuery; };
	const Config::EventsToSuppress& GetEventsToSuppress() { return m_eventsToSuppress; };

	CJSONConfig();
	virtual ~CJSONConfig();

};

