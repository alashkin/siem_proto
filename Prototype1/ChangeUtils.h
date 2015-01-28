#pragma once

#include <Windows.h>
#include <string>
#include <map>
#include <set>
#include <atlsecurity.h>
#include "Log.h"
#include "EvtConvert.h"
#include "macroses.h"
#include <vector>

namespace ChangeUtils
{
	namespace Consts
	{
		enum ChangeInfo
		{
			WHO_DomainNetbios = 0,
			WHO_DomainFQDN,
			WHO_Name,
			WHO_Sid,
			WHAT_DN,
			WHAT_Class,
			WHAT_Guid,
			WHEN,
			WHERE,
			WHAT_Name,
			CI_PROPS_COUNT
		};


	}

	struct ObjectChange
	{
		//++ change type
		enum ChangeType { ctNone, ctObjectDeleted, ctObjectModified, ctObjectCreated, ctObjectMoved, ctObjectUndeleted, ctEventLogCleared, ctMaxID };
		ChangeType changeType;
		//--

		//++ general values (WHO-WHEN-WHERE etc & meta information)
		struct ChangeInfo
		{
			struct Meta
			{
				Meta() : eventRecordID(0xFFFFFFFFFFFFFFFF), timeCreated(0xFFFFFFFFFFFFFFFF) {};
				std::wstring CID;
				UINT64 eventRecordID;  
				UINT64 timeCreated;
			} meta;

			std::wstring data[Consts::CI_PROPS_COUNT];
		} changeInfo;
		//--

		//++ Atttribute change description
		struct AttributeValue
		{
			struct Meta
			{
				Meta() : timeDetected(0), eventRecordID(0) {};
				UINT64 timeDetected;
				UINT64 eventRecordID;
			} meta;

			std::wstring rawValue;
			std::wstring parsedValue;
		};

		struct PredAttributeLess
		{
			bool operator() (const AttributeValue& attr1, const AttributeValue& attr2) const
			{
				return (attr1.meta.timeDetected != attr2.meta.timeDetected) ?
					(attr1.meta.timeDetected < attr2.meta.timeDetected) : (attr1.meta.eventRecordID < attr2.meta.eventRecordID);
			}
		};

		typedef std::set<AttributeValue, PredAttributeLess> attribute_values_t;

		struct AttributeChange
		{
			attribute_values_t oldValues;
			attribute_values_t newValues;
			std::wstring hrt;
		};

		typedef std::map<std::wstring, AttributeChange> attributes_t;
		attributes_t changedAttributes;
		//--
	};

	inline bool operator < (const ObjectChange& obj1, const ObjectChange& obj2)
	{
		return (obj1.changeInfo.meta.timeCreated != obj2.changeInfo.meta.timeCreated) ? 
							obj1.changeInfo.meta.timeCreated < obj2.changeInfo.meta.timeCreated :
							obj1.changeInfo.meta.eventRecordID < obj2.changeInfo.meta.eventRecordID;
	}

	typedef std::vector<std::shared_ptr<ObjectChange>> object_changes_t;

	void DumpObjectChange(const ObjectChange& change);
}

