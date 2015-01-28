#include "stdafx.h"
#include "JSONConfig.h"
#include "macroses.h"
#include "Log.h"
#include <boost/lexical_cast.hpp>
#include <json_spirit/json_spirit_reader_template.h>
#include <fstream>
#include "unicode.h"
#include <atlbase.h>
#include <boost/algorithm/string.hpp>

namespace
{
	template <class T > void CheckType(const T& t, json_spirit::Value_type vt)
	{
		if (t.type() != vt)
		{
			throw std::invalid_argument("JSON type mismatch. Expected: " + json_spirit::value_type_to_string(vt)
				+ ", received: " + json_spirit::value_type_to_string(t.type()) + ".");
		}
	}


	template <class Object>
		const json_spirit::wmValue& SafeGetElement(const Object& obj, json_spirit::Value_type vt, const std::wstring& key)
	{
		auto elem = obj.find(key);
		if (elem == obj.end())
		{
			throw std::out_of_range("JSON: Cannot find " + boost::lexical_cast<ansi::string>(key) + " element");
		}

		CheckType(elem->second, vt);
		return elem->second;
	}
}

CJSONConfig::CJSONConfig()
{
}

void CJSONConfig::LoadTargets(const json_spirit::wmObject& objRoot)
{
	_LOG_PREPARE_REC();


	const json_spirit::wmArray& arrTgts = SafeGetElement(objRoot, json_spirit::array_type, L"targets").get_array();

	for (unsigned  int i = 0; i < arrTgts.size(); ++i)
	{
		_LOG_OPEN_REC(info);

		_LOG_ADD_TO_REC() << "Target #" << i;

		Config::target_t tgt;

		const json_spirit::wmValue& valTgt = arrTgts[i];
		CheckType(valTgt, json_spirit::obj_type);

		const json_spirit::wmObject& objTgt = valTgt.get_obj();

 		tgt.server = SafeGetElement(objTgt, json_spirit::str_type, L"server").get_str();
		_LOG_ADD_TO_REC() << " server = " << tgt.server;

		tgt.domain = SafeGetElement(objTgt, json_spirit::str_type, L"domain").get_str();
		_LOG_ADD_TO_REC() << " domain = " << tgt.domain;

		tgt.user = SafeGetElement(objTgt, json_spirit::str_type, L"user").get_str();
		_LOG_ADD_TO_REC() << " user = " << tgt.user;

		tgt.password = SafeGetElement(objTgt, json_spirit::str_type, L"password").get_str();
		_LOG_ADD_TO_REC() << " password = " << tgt.password;

		tgt.loginType = SafeGetElement(objTgt, json_spirit::int_type, L"login_type").get_int();
		_LOG_ADD_TO_REC() << " login_type = " << tgt.loginType;

		tgt.network_timeout = SafeGetElement(objTgt, json_spirit::int_type, L"network_timeout").get_int();
		_LOG_ADD_TO_REC() << " network_timeout = " << tgt.loginType;

		tgt.max_event_tolerance = SafeGetElement(objTgt, json_spirit::int_type, L"max_event_tolerance").get_int();
		_LOG_ADD_TO_REC() << " max_event_tolerance = " << tgt.max_event_tolerance;

		tgt.wait_for_event_min = SafeGetElement(objTgt, json_spirit::int_type, L"wait_for_event_min").get_int();
		_LOG_ADD_TO_REC() << " wait_for_event_min = " << tgt.wait_for_event_min;

		tgt.wait_for_event_max = SafeGetElement(objTgt, json_spirit::int_type, L"wait_for_event_max").get_int();
		_LOG_ADD_TO_REC() << " wait_for_event_max = " << tgt.wait_for_event_max;

		tgt.initial_chunk_size = SafeGetElement(objTgt, json_spirit::int_type, L"initial_chunk_size").get_int();
		_LOG_ADD_TO_REC() << " initial_chunk_size = " << tgt.initial_chunk_size;
		_LOG_CLOSE_REC();

		if (m_targets.find(tgt) != m_targets.end())
		{
			throw std::invalid_argument("Duplicated target: '" + boost::lexical_cast<ansi::string>(tgt.server) + "'");
		}


		m_targets.insert(tgt);
	}
}

void CJSONConfig::LoadSettings(const json_spirit::wmObject& objRoot)
{
	_LOG_PREPARE_REC();

	const json_spirit::wmObject& objSettings = SafeGetElement(objRoot, json_spirit::obj_type, L"settings").get_obj();
	const json_spirit::wmObject& objADChema = SafeGetElement(objSettings, json_spirit::obj_type, L"ad_schema").get_obj();

	_LOG_ADD_TO_REC() << L"AD schema load params: ";

	m_settings.adSchema.server = SafeGetElement(objADChema, json_spirit::str_type, L"server").get_str();
	_LOG_ADD_TO_REC() << " server = " << m_settings.adSchema.server;

	m_settings.adSchema.domain_fqdn = SafeGetElement(objADChema, json_spirit::str_type, L"domain_fqdn").get_str();
	_LOG_ADD_TO_REC() << " domain_fqdn = " << m_settings.adSchema.domain_fqdn;

	m_settings.adSchema.user = SafeGetElement(objADChema, json_spirit::str_type, L"user").get_str();
	_LOG_ADD_TO_REC() << " user = " << m_settings.adSchema.user;

	m_settings.adSchema.password = SafeGetElement(objADChema, json_spirit::str_type, L"password").get_str();
	_LOG_ADD_TO_REC() << " password = " << m_settings.adSchema.password;

	_LOG_CLOSE_REC();
}

void CJSONConfig::LoadLogsAndEvents(const json_spirit::wmObject& objRoot)
{
	_LOG_PREPARE_REC();

	const json_spirit::wmObject& objLE = SafeGetElement(objRoot, json_spirit::obj_type, L"logs_and_events").get_obj();

	bool bEvents = false;
	bool bFile = false;
	if (objLE.find(L"log_and_ids") != objLE.end())
	{
		bEvents = true;

		const json_spirit::wmArray& arrLaE = SafeGetElement(objLE, json_spirit::array_type, L"log_and_ids").get_array();

		for (unsigned int i = 0; i < arrLaE.size(); ++i)
		{
			const json_spirit::wmValue& vLog = arrLaE[i];
			CheckType(vLog, json_spirit::obj_type);

			const std::wstring& strLogName = SafeGetElement(vLog.get_obj(), json_spirit::str_type, L"log_name").get_str();

			if (m_events.find(strLogName) != m_events.end())
			{
				throw std::invalid_argument("Duplicated log_name: " + boost::lexical_cast<ansi::string>(strLogName)+" .");
			}

			_LOG_OPEN_REC(info);

			_LOG_ADD_TO_REC() << "Events to collect from " << strLogName << " log: ";

			// loading events id
			Config::events_t evts;
			const json_spirit::wmArray& arrEvents = SafeGetElement(vLog.get_obj(), json_spirit::array_type, L"events").get_array();

			if (arrEvents.size() == 1 && arrEvents[0].type() == json_spirit::str_type &&
				boost::iequals(arrEvents[0].get_str(), "*"))
			{
				_LOG_ADD_TO_REC() << "!!! ALL !!!";
			}
			else
			{
				for (unsigned int j = 0; j < arrEvents.size(); ++j)
				{
					const json_spirit::wmValue& vID = arrEvents[j];
					CheckType(vID, json_spirit::int_type);

					if (evts.find(vID.get_int()) != evts.end())
					{
						throw std::invalid_argument("Duplicated EventID: " + boost::lexical_cast<ansi::string>(vID.get_int()));
					}

					evts.insert(vID.get_int());

					_LOG_ADD_TO_REC() << " " << vID.get_int();
				}
			}

			m_events[strLogName] = evts;

			_LOG_CLOSE_REC();
		}

		m_maxIDinQuery = SafeGetElement(objLE, json_spirit::int_type, L"num_id_in_query").get_int();

	}


	const json_spirit::wmObject& objEventsToSuppress = SafeGetElement(objLE, json_spirit::obj_type, L"events_to_suppress").get_obj();
	const json_spirit::wmArray& arrEvent_data_51xx = SafeGetElement(objEventsToSuppress, json_spirit::array_type, L"event_data_51xx").get_array();

	for (unsigned int i = 0; i < arrEvent_data_51xx.size(); i++)
	{
		const json_spirit::wmValue& ets = arrEvent_data_51xx[i];
		CheckType(ets, json_spirit::obj_type);

		const json_spirit::wmObject& etsObj = ets.get_obj();

		Config::EventsToSuppress::and_map_t and_map;
		for (auto& av : etsObj)
		{
			and_map[av.first] = av.second.get_str();
		}

		m_eventsToSuppress.event_data_51xx_filter.push_back(and_map);
	}

	if (objLE.find(L"load_from") != objLE.end())
	{
		bFile = true;
		m_queryFile = SafeGetElement(objLE, json_spirit::str_type, L"load_from").get_str();
	}

	if (bFile && bEvents)
	{
		throw std::invalid_argument("'load_from' and 'log_and_ids' can not be used at once");
	}

	if (!bFile && !bEvents)
	{
		throw std::invalid_argument("Neither 'load_from' nor 'log_and_ids' specified");
	}

	m_queryType = bFile ? qtFile : qtEvents;


}

void CJSONConfig::LoadAll(const json_spirit::wmValue& value)
{
	// root have to be "object"
	CheckType(value, json_spirit::obj_type);
	const json_spirit::wmObject& objRoot = value.get_obj();

	LoadLogsAndEvents(objRoot);
	LoadTargets(objRoot);
	LoadSettings(objRoot);
}


void CJSONConfig::Init(const std::wstring& filename) // throw (std::fstream::failure)
{
	std::ifstream cfg;
	cfg.exceptions(std::ifstream::failbit | std::ifstream::badbit);

	cfg.open(filename, std::ios_base::in);

	json_spirit::wmValue value;
	json_spirit::read_stream(cfg, value);

	if (value.is_null()) throw std::fstream::failure("Invalid JSON config");

	LoadAll(value);
}

CJSONConfig::~CJSONConfig()
{
}
