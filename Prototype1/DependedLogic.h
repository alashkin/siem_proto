#pragma once

#include "ChangeUtils.h"
#include "EventUtils.h"

namespace DependedLogic
{
	bool AllowToleranceMatching(ChangeUtils::ObjectChange::ChangeType ct);

	bool AcceptEvent(ChangeUtils::ObjectChange::ChangeType ct, const EventUtils::ParsedEvent1& evt );

	ChangeUtils::ObjectChange::ChangeType GetChangeType(ChangeUtils::ObjectChange::ChangeType ct, 
																	const EventUtils::ParsedEvent1& evt);

	void LoadChangeInfo(ChangeUtils::ObjectChange::ChangeType ct, const EventUtils::ParsedEvent1& evt,
																	ChangeUtils::ObjectChange* objectChange);
}