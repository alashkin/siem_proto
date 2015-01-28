#pragma once

#include <string>
#include <vector>
#include <map>
#include "ChangeUtils.h"
#include "EventUtils.h"

namespace DependedLogicDefines
{
	struct ChangesDependedInformation
	{
		ChangeUtils::ObjectChange::ChangeType changeType;
		std::wstring changeName;
		bool allowToleranceMatching;
		std::vector<UINT16> acceptedEvents;
		std::map<UINT16, ChangeUtils::ObjectChange::ChangeType> transformations;
		std::vector<std::pair<ChangeUtils::Consts::ChangeInfo, std::wstring>> ciLoadInfo;
		bool fillWhatName;
		ChangesDependedInformation(ChangeUtils::ObjectChange::ChangeType _changeType,
			const std::wstring& _changeName,
			bool _allowToleranceMatching,
			const std::vector<UINT16>& _acceptedEvents,
			const std::map<UINT16, ChangeUtils::ObjectChange::ChangeType>& _transformations,
			const std::vector<std::pair<ChangeUtils::Consts::ChangeInfo, std::wstring>>& _ciLoadInfo,
			bool _fillWhatName
			) :
			changeType(_changeType),
			changeName(_changeName),
			allowToleranceMatching(_allowToleranceMatching),
			acceptedEvents(_acceptedEvents),
			transformations(_transformations),
			ciLoadInfo(_ciLoadInfo),
			fillWhatName(_fillWhatName) {};
	};

#define CI_LOAD(const_name) { ChangeUtils::Consts::ChangeInfo::const_name, EventUtils::Consts::EventData::const_name }

#define NSTD_CI_LOAD(const_name1, const_name2) { ChangeUtils::Consts::ChangeInfo::const_name1, EventUtils::Consts::EventData::const_name2 }

#define STD_CI_NOWHATDN_LOAD \
	CI_LOAD(WHO_DomainNetbios), \
	CI_LOAD(WHO_DomainFQDN), \
	CI_LOAD(WHO_Name), \
	CI_LOAD(WHAT_Class), \
	CI_LOAD(WHAT_Guid), \
	CI_LOAD(WHO_Sid)

#define STD_CI_LOAD \
	STD_CI_NOWHATDN_LOAD, \
	CI_LOAD(WHAT_DN)

#define TRNSFM(const_name) { EventUtils::Consts::ID::EID_##const_name, ChangeUtils::ObjectChange::ChangeType::ct##const_name }
}