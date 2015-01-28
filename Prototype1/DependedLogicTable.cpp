#include "stdafx.h"
#include "DependedLogicDefines.h"

DependedLogicDefines::ChangesDependedInformation cdi[ChangeUtils::ObjectChange::ChangeType::ctMaxID] =
{
	{
		ChangeUtils::ObjectChange::ChangeType::ctNone,
		L"ctNone",
		false,
		{},
		{
			TRNSFM(ObjectCreated),
			TRNSFM(ObjectDeleted),
			TRNSFM(ObjectUndeleted),
			TRNSFM(EventLogCleared),
			TRNSFM(ObjectMoved),
			TRNSFM(ObjectModified)
		},
		{},
		false
	},
	{
		ChangeUtils::ObjectChange::ChangeType::ctObjectDeleted,
		L"ctObjectDeleted",
		false,
		{},
		{},
		{ STD_CI_LOAD },
		true
	},
	{
		ChangeUtils::ObjectChange::ChangeType::ctObjectModified,
		L"ctObjectModified",
		false,
		{ EventUtils::Consts::ID::EID_ObjectModified },
		{
			TRNSFM(ObjectCreated),
			TRNSFM(ObjectUndeleted)
		},
		{ STD_CI_LOAD },
		true
	},
	{
		ChangeUtils::ObjectChange::ChangeType::ctObjectCreated,
		L"ctObjectCreated",
		true,
		{ EventUtils::Consts::ID::EID_ObjectModified },
		{},
		{ STD_CI_LOAD },
		true
	},
	{
		ChangeUtils::ObjectChange::ChangeType::ctObjectMoved,
		L"ctObjectMoved",
		false,
		{},
		{},
		{
			NSTD_CI_LOAD(WHAT_DN, OldDN),
			STD_CI_NOWHATDN_LOAD
		},
		true
	},
	{
		ChangeUtils::ObjectChange::ChangeType::ctObjectUndeleted,
		L"ctObjectUndeleted",
		true,
		{ EventUtils::Consts::ID::EID_ObjectModified },
		{},
		{
			NSTD_CI_LOAD(WHAT_DN, NewDN),
			STD_CI_NOWHATDN_LOAD
		},
		true
	},
	{
		ChangeUtils::ObjectChange::ChangeType::ctEventLogCleared,
		L"ctEventLogCleared",
		false,
		{},
		{},
		{
			CI_LOAD(WHO_Name),
			CI_LOAD(WHO_Sid),
			CI_LOAD(WHO_DomainNetbios)
		},
		false
	}
};

