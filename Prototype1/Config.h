#pragma once

#include <set>
#include <map>
#include <string>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <vector>

namespace Config
{
	class IBookmarkStorage
	{
	public:
		virtual std::wstring Load() = 0;
		virtual void Save(const std::wstring&) = 0;
	};


	struct Target
	{
		std::wstring server;
		std::wstring user;
		std::wstring domain;
		std::wstring password;
		unsigned long loginType;
		unsigned int network_timeout;
		unsigned int max_event_tolerance;
		unsigned int wait_for_event_min;
		unsigned int wait_for_event_max;
		unsigned int initial_chunk_size;
	};

	struct Settings
	{
		struct ADSchema
		{
			std::wstring server;
			std::wstring domain_fqdn;
			std::wstring user;
			std::wstring password;
		} adSchema;
	};
	inline bool operator < (const Target& tgt1, const Target& tgt2) 
	{
		return tgt1.server < tgt2.server;
	}

	typedef std::set<unsigned int> events_t;
	typedef std::map< std::wstring, events_t> events_to_collect_t;
	typedef std::set<std::wstring>  properties_to_collect_t;
	typedef Target target_t;
	typedef std::set<target_t> targets_t;

	struct EventsToSuppress
	{
		typedef std::map<std::wstring, std::wstring> and_map_t;
		typedef std::vector<and_map_t> or_vector_t;

		or_vector_t event_data_51xx_filter;
	};
}