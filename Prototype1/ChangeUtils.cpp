#include "stdafx.h"

#include "ChangeUtils.h"

namespace
{
	std::wstring CIPropIDToString(ChangeUtils::Consts::ChangeInfo id)
	{
		using namespace ChangeUtils::Consts;

		static std::map<ChangeInfo, std::wstring> names_map{
		 DECLARE_MIRR_CONST_NAME(WHO_DomainNetbios),			
		 DECLARE_MIRR_CONST_NAME(WHO_DomainFQDN),         
		 DECLARE_MIRR_CONST_NAME(WHO_Name),               
		 DECLARE_MIRR_CONST_NAME(WHO_Sid),                
		 DECLARE_MIRR_CONST_NAME(WHAT_DN),                    
		 DECLARE_MIRR_CONST_NAME(WHAT_Class),                    
		 DECLARE_MIRR_CONST_NAME(WHAT_Guid),                  
		 DECLARE_MIRR_CONST_NAME(WHEN),                 
		 DECLARE_MIRR_CONST_NAME(WHERE),  
		 DECLARE_MIRR_CONST_NAME(WHAT_Name)
	};

		return names_map[id];
	}

	void DumpAttributeValue(ChangeUtils::ObjectChange::attribute_values_t& values,
		const std::wstring& empty,
		const std::wstring& oneValue,
		const std::wstring& manyValues)
	{
		if (values.empty())
		{
			_LOG(debug) << empty;
		}
		else if (values.size() == 1)
		{
			if (values.begin()->parsedValue.empty())
			{
				_LOG(debug) << oneValue << values.begin()->rawValue;
			}
			else
			{
				_LOG(debug) << oneValue << values.begin()->parsedValue << L" (" << values.begin()->rawValue << L")";
			}
		}
		else
		{
			_LOG(debug) << manyValues;
			int i = 1;
			for (auto& attr_value : values)
			{
				if (attr_value.parsedValue.empty())
				{
					_LOG(debug) << L"### " << i++ << L": " << attr_value.rawValue;
				}
				else
				{
					_LOG(debug) << L"### " << i++ << L": " << attr_value.parsedValue << L" (" << attr_value.rawValue << ")";
				}
			}
		}

	}
}

void ChangeUtils::DumpObjectChange(const ObjectChange& change)
{
	const wchar_t* changeTypeString[] = { L"", L"ObjectDeleted", L"ObjectModified", 
		L"ObjectCreated", L"ObjectMoved", L"ObjectUndeleted", L"EventLogCleared" };

	_LOG(debug) << L"# Active Directory change detected #";
	_LOG(debug) << L"# ChangeType: " << changeTypeString[change.changeType];

	for (int i = 0; i < Consts::ChangeInfo::CI_PROPS_COUNT; i++ )
	{
		_LOG(debug) << L"# " << CIPropIDToString(Consts::ChangeInfo(i)) << L" : " << change.changeInfo.data[i];
	}

	for (auto attr : change.changedAttributes)
	{
		_LOG(debug) << L"## Changed attribute: " << attr.first;

		DumpAttributeValue(attr.second.oldValues, L"### Old value: <empty>", L"### Old value: ", L"### Old values: ");
		DumpAttributeValue(attr.second.newValues, L"### New value: <empty>", L"### New value: ", L"### New values: ");

		if (!attr.second.hrt.empty())
		{
			_LOG(debug) << L"#### Change: " << attr.second.hrt;
		}
	}
}
