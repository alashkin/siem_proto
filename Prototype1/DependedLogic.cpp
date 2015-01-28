#include "stdafx.h"
#include "DependedLogic.h"
#include "utils.h"
#include "ChangeUtils.h"
#include "EventUtils.h"
#include <string>
#include <vector>
#include <map>
#include "DependedLogicDefines.h"
#include "ad_utils.h"

extern DependedLogicDefines::ChangesDependedInformation cdi[ChangeUtils::ObjectChange::ChangeType::ctMaxID];

bool DependedLogic::AllowToleranceMatching(ChangeUtils::ObjectChange::ChangeType ct)
{
	return cdi[ct].allowToleranceMatching;
}

bool DependedLogic::AcceptEvent(ChangeUtils::ObjectChange::ChangeType ct, const EventUtils::ParsedEvent1& evt)
{
	DependedLogicDefines::ChangesDependedInformation& depInfo = cdi[ct];
	UINT16 eid = boost::get<UINT16>(evt.system->data[::EvtSystemEventID]);

	for (auto ac_eid : depInfo.acceptedEvents)
	{
		if (ac_eid == eid)
		{
			return true;
		}
	}

	return false;
}

ChangeUtils::ObjectChange::ChangeType DependedLogic::GetChangeType(ChangeUtils::ObjectChange::ChangeType ct,
																					const EventUtils::ParsedEvent1& evt)
{
	ChangeUtils::ObjectChange::ChangeType retVal = ct;
	DependedLogicDefines::ChangesDependedInformation& depInfo = cdi[ct];
	UINT16 eid = boost::get<UINT16>(evt.system->data[::EvtSystemEventID]);
	
	auto trnsm = depInfo.transformations.find(eid);
	if (trnsm != depInfo.transformations.end())
	{
		retVal = trnsm->second;
	}

	return retVal;
}

void DependedLogic::LoadChangeInfo(ChangeUtils::ObjectChange::ChangeType ct, const EventUtils::ParsedEvent1& evt,
																					ChangeUtils::ObjectChange* objectChange)
{
	UINT16 eid = boost::get<UINT16>(evt.system->data[::EvtSystemEventID]);

	// system fields
	utils::ConvertUINT64FTtoString(boost::get<UINT64>(evt.system->data[::EvtSystemTimeCreated]),
								&objectChange->changeInfo.data[ChangeUtils::Consts::ChangeInfo::WHEN]);

	objectChange->changeInfo.data[ChangeUtils::Consts::ChangeInfo::WHERE]
							= boost::get<std::wstring>(evt.system->data[::EvtSystemComputer]);

	// event/user fields
	DependedLogicDefines::ChangesDependedInformation& depInfo = cdi[ct];
	
	for (auto& ci : depInfo.ciLoadInfo)
	{
		objectChange->changeInfo.data[ci.first] = evt.eventData->at(ci.second);
	}

	if (depInfo.fillWhatName)
	{
		objectChange->changeInfo.data[ChangeUtils::Consts::WHAT_Name] =
				ADUtils::ExtractNameFromDN(objectChange->changeInfo.data[ChangeUtils::Consts::WHAT_DN]);
	}
}
															
