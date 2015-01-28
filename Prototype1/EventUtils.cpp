#include "stdafx.h"
#include "EventUtils.h"
#include "EvtConvert.h"
#include "Log.h"
#include "macroses.h"

namespace
{
	std::wstring EvtSystemPropIDToString(EVT_SYSTEM_PROPERTY_ID id)
	{
		static std::map<EVT_SYSTEM_PROPERTY_ID, std::wstring> names_map{
		DECLARE_MIRR_CONST_NAME(EvtSystemProviderName),				// EvtVarTypeString             
		DECLARE_MIRR_CONST_NAME(EvtSystemProviderGuid),              // EvtVarTypeGuid  
		DECLARE_MIRR_CONST_NAME(EvtSystemEventID),                   // EvtVarTypeUInt16  
		DECLARE_MIRR_CONST_NAME(EvtSystemQualifiers),                // EvtVarTypeUInt16
		DECLARE_MIRR_CONST_NAME(EvtSystemLevel),                     // EvtVarTypeUInt8
		DECLARE_MIRR_CONST_NAME(EvtSystemTask),                      // EvtVarTypeUInt16
		DECLARE_MIRR_CONST_NAME(EvtSystemOpcode),                    // EvtVarTypeUInt8
		DECLARE_MIRR_CONST_NAME(EvtSystemKeywords),                  // EvtVarTypeHexInt64
		DECLARE_MIRR_CONST_NAME(EvtSystemTimeCreated),               // EvtVarTypeFileTime
		DECLARE_MIRR_CONST_NAME(EvtSystemEventRecordId),             // EvtVarTypeUInt64
		DECLARE_MIRR_CONST_NAME(EvtSystemActivityID),                // EvtVarTypeGuid
		DECLARE_MIRR_CONST_NAME(EvtSystemRelatedActivityID),         // EvtVarTypeGuid
		DECLARE_MIRR_CONST_NAME(EvtSystemProcessID),                 // EvtVarTypeUInt32
		DECLARE_MIRR_CONST_NAME(EvtSystemThreadID),                  // EvtVarTypeUInt32
		DECLARE_MIRR_CONST_NAME(EvtSystemChannel),                   // EvtVarTypeString 
		DECLARE_MIRR_CONST_NAME(EvtSystemComputer),                  // EvtVarTypeString 
		DECLARE_MIRR_CONST_NAME(EvtSystemUserID),                    // EvtVarTypeSid
		DECLARE_MIRR_CONST_NAME(EvtSystemVersion),                   // EvtVarTypeUInt8
		DECLARE_MIRR_CONST_NAME(EvtSystemPropertyIdEND)
	};

		return names_map[id];
	}

	class DumpEventVisitor : public boost::static_visitor<>
	{
	public:
		DumpEventVisitor(_LOG_REC_TYPE& rec) : m_logRec(rec) {};

		void operator()(UINT8 val) const
		{
			m_logRec << (int)val;
		}

		void operator()(UINT16 val) const
		{
			m_logRec << val;
		}

		void operator()(UINT32 val) const
		{
			m_logRec << val;
		}

		void operator()(UINT64 val) const
		{
			m_logRec << val;
		}

		void operator()(const std::wstring& val) const
		{
			m_logRec << val;
		}

		void operator()(const GUID& val) const
		{
			m_logRec << boost::lexical_cast<std::wstring>(val);
		}

		void operator()(const ATL::CSid& val) const
		{
			m_logRec << boost::lexical_cast<std::wstring>(val);
		}

	private:
		_LOG_REC_TYPE& m_logRec;

	};
}

void EventUtils::DumpEvent(EventUtils::ParsedEvent1& pe)
{
	_LOG(debug) << L"* Event Collected *";
	_LOG_PREPARE_REC();

	for (int i = 0; i < ::EvtSystemPropertyIdEND; i++ )
	{
		_LOG_OPEN_REC(debug);
		_LOG_ADD_TO_REC() << L"** " << EvtSystemPropIDToString(EVT_SYSTEM_PROPERTY_ID(i)) << L" : ";

		if (i != ::EvtSystemTimeCreated)
		{
			boost::apply_visitor(DumpEventVisitor(_LOG_REC()), pe.system->data[i] );
		}
		else
		{
			_LOG_ADD_TO_REC() << boost::lexical_cast<std::wstring>(*((FILETIME*)&boost::get<UINT64>(pe.system->data[i])));
		}
		_LOG_CLOSE_REC();
	}

	for (auto& add_info : *pe.eventData.get())
	{
		_LOG(debug) << L"*** " << add_info.first << L" : " << add_info.second;
	}
}


namespace
{
	template <class T, class D> T GetValue(const EVT_VARIANT& evt, T* t, D* d)
	{
		return (evt.Type != EvtVarTypeNull && t != 0) ? *t : *d;
	}

	template <class T, class D> D GetValue(const EVT_VARIANT& evt, T* t, const D& d)
	{
		return (evt.Type != EvtVarTypeNull && t != 0) ? D(t) : d;
	}

	template <class T, class D> T GetValue(const EVT_VARIANT& evt, T& t, const D& d)
	{
		return (evt.Type != EvtVarTypeNull ) ? t : d;
	}

}

std::shared_ptr<EventUtils::EventSystemProperties> EventUtils::UnwrapSystemProperties(const EVT_VARIANT* systemProps)
{
	std::shared_ptr<EventUtils::EventSystemProperties> retVal(new EventUtils::EventSystemProperties());
	static GUID _nG = { 0 }; 
	static GUID* nullGUID = &_nG;
	static std::wstring nullString(L"");
	static UINT16 nullUINT16 = 0;
	static UINT8 nullUINT8 = 0;
	static UINT64 nullUINT64 = 0;
	static UINT32 nullUINT32 = 0;

#define UnwrapValue(name, val, def) 	retVal->data[name] = GetValue(systemProps[name], systemProps[name].val, def);

	UnwrapValue(EvtSystemProviderName, StringVal, nullString);
	UnwrapValue(EvtSystemProviderGuid, GuidVal, nullGUID);
	UnwrapValue(EvtSystemEventID, UInt16Val, nullUINT16);
	UnwrapValue(EvtSystemQualifiers, UInt16Val, nullUINT16);
	UnwrapValue(EvtSystemLevel, ByteVal, nullUINT8);
	UnwrapValue(EvtSystemTask, UInt16Val, nullUINT16);
	UnwrapValue(EvtSystemOpcode, ByteVal, nullUINT8);
	UnwrapValue(EvtSystemKeywords, UInt64Val, nullUINT64);
	UnwrapValue(EvtSystemEventRecordId, UInt64Val, nullUINT64);
	UnwrapValue(EvtSystemActivityID, GuidVal, nullGUID);
	UnwrapValue(EvtSystemRelatedActivityID, GuidVal, nullGUID);
	UnwrapValue(EvtSystemProcessID, UInt32Val, nullUINT32);
	UnwrapValue(EvtSystemThreadID, UInt32Val, nullUINT32);
	UnwrapValue(EvtSystemChannel, StringVal, nullString);
	UnwrapValue(EvtSystemComputer, StringVal, nullString);
	UnwrapValue(EvtSystemVersion, ByteVal, nullUINT8);

	UINT64 timeCreated = (systemProps[EvtSystemTimeCreated].Type != EvtVarTypeNull) ? *(UINT64*)(&systemProps[EvtSystemTimeCreated].FileTimeVal): nullUINT64;
	retVal->data[EvtSystemTimeCreated] = timeCreated;

	ATL::CSid userID = (systemProps[EvtSystemUserID].Type != EvtVarTypeNull) ? ATL::CSid((SID*)systemProps[EvtSystemUserID].SidVal) : CSid();
	retVal->data[EvtSystemUserID]= userID;

	return retVal;
}
