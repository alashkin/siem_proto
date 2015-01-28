#pragma once

#include <windows.h>
#include <string>
#include <atlsecurity.h>
#include <winevt.h>
#include <map>
#include <memory>
#include <boost/variant.hpp>

namespace EventUtils
{

	struct EventSystemProperties
	{
		typedef boost::variant<std::wstring, GUID, CSid, UINT8, UINT16, UINT32, UINT64>  property_value_t;
		property_value_t data[::EvtSystemPropertyIdEND];
	};

	std::shared_ptr<EventSystemProperties> UnwrapSystemProperties(const EVT_VARIANT* systemProps);

	struct RawEvent1
	{
		std::shared_ptr<EventUtils::EventSystemProperties> system;
		std::shared_ptr<std::wstring> eventXML;
	};

	struct ParsedEvent1
	{
		std::shared_ptr<EventUtils::EventSystemProperties> system;

		typedef std::map<std::wstring, std::wstring> eventdata_t;
		std::shared_ptr<eventdata_t> eventData;
	};

	namespace Consts
	{
		namespace ID
		{
			const UINT16 EID_ObjectCreated = 5137;
			const UINT16 EID_ObjectModified = 5136;
			const UINT16 EID_ObjectDeleted = 5141;
			const UINT16 EID_ObjectMoved = 5139;
			const UINT16 EID_ObjectUndeleted = 5138;
			const UINT16 EID_EventLogCleared = 1102;
		}

		namespace EventData
		{
			const std::wstring CID(L"OpCorrelationID");
			const std::wstring WHO_DomainNetbios(L"SubjectDomainName");
			const std::wstring WHO_DomainFQDN(L"DSName");
			const std::wstring WHO_Name(L"SubjectUserName");
			const std::wstring WHO_Sid(L"SubjectUserSid");
			const std::wstring WHAT_DN(L"ObjectDN");
			const std::wstring WHAT_Class(L"ObjectClass");
			const std::wstring WHAT_Guid(L"ObjectGUID");
			const std::wstring OperationType(L"OperationType");
			const std::wstring AttributeName(L"AttributeLDAPDisplayName");
			const std::wstring AttributeValue(L"AttributeValue");

			const std::wstring OldDN(L"OldObjectDN");
			const std::wstring NewDN(L"NewObjectDN");
		}

		namespace Values
		{
			const std::wstring OpTypeAdded(L"%%14674");
			const std::wstring OpTypeDeleted(L"%%14675");
		}

	}

	void DumpEvent(EventUtils::ParsedEvent1& pe);

}
