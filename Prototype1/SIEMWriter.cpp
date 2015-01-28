#include "stdafx.h"
#include "SIEMWriter.h"

#include <windows.h>
#include <stdio.h>
#include <evntprov.h>

#include "etw_provider.h"
#include <string>
#include <vector>
#include <stdexcept>
#include "utils.h"

CSIEMWriter::CSIEMWriter()
{
}


CSIEMWriter::~CSIEMWriter()
{
}

void CSIEMWriter::WriteEvent(const ChangeUtils::ObjectChange& objectChange)
{

	REGHANDLE registrationHandle = NULL;
	EVENT_DATA_DESCRIPTOR descriptors[100] = { 0 };
	DWORD status = ERROR_SUCCESS;
	ULONG currDescr = 0, descrCount = 0;

	if ((status = ::EventRegister(&KAPAAI_PROTOTYPE1, NULL,
										NULL, &registrationHandle)) != ERROR_SUCCESS)
	{
		throw std::runtime_error(utils::FormatLastErrorA("EventRegister"));
	}
	// %1 - change type
	USHORT changeType = objectChange.changeType;
	::EventDataDescCreate(&descriptors[currDescr++], &changeType, sizeof(USHORT));

	static const ChangeUtils::Consts::ChangeInfo ci_fields[ChangeUtils::Consts::ChangeInfo::CI_PROPS_COUNT] =
	{
		ChangeUtils::Consts::ChangeInfo::WHO_DomainNetbios, //%2
		ChangeUtils::Consts::ChangeInfo::WHO_DomainFQDN, // %3
		ChangeUtils::Consts::ChangeInfo::WHO_Name, // %4
		ChangeUtils::Consts::ChangeInfo::WHO_Sid, // %5
		ChangeUtils::Consts::ChangeInfo::WHAT_DN, // %6
		ChangeUtils::Consts::ChangeInfo::WHAT_Name, //%7
		ChangeUtils::Consts::ChangeInfo::WHAT_Class, //%8
		ChangeUtils::Consts::ChangeInfo::WHAT_Guid, //%9
		ChangeUtils::Consts::ChangeInfo::WHEN, //%10
		ChangeUtils::Consts::ChangeInfo::WHERE //%11
	};

	for (auto ci_field : ci_fields)
	{
		::EventDataDescCreate(&descriptors[currDescr++], objectChange.changeInfo.data[ci_field].c_str(), (objectChange.changeInfo.data[ci_field].size() + 1) * sizeof(wchar_t));
	}

	// %12
	USHORT changedAttrubutesCount = objectChange.changedAttributes.size();
	::EventDataDescCreate(&descriptors[currDescr++], &changedAttrubutesCount, sizeof(changedAttrubutesCount));

	// %13

	std::wstring hrt;

	for (auto& attr : objectChange.changedAttributes)
	{
		// name
		::EventDataDescCreate(&descriptors[currDescr++], attr.first.c_str(), (attr.first.size() + 1) * sizeof(wchar_t));

		// oldvaluecount
		USHORT oldValueCount = attr.second.oldValues.size();
		::EventDataDescCreate(&descriptors[currDescr++], &oldValueCount, sizeof(oldValueCount));

		// oldvalues
		for (auto& ov : attr.second.oldValues)
		{
			::EventDataDescCreate(&descriptors[currDescr++], ov.parsedValue.c_str(), (ov.parsedValue.size() + 1) * sizeof(wchar_t));
		}

		USHORT newValueCount = attr.second.newValues.size();
		::EventDataDescCreate(&descriptors[currDescr++], &newValueCount, sizeof(newValueCount));

		// newvaluecount
		for (auto& nv : attr.second.newValues)
		{
			::EventDataDescCreate(&descriptors[currDescr++], nv.parsedValue.c_str(), (nv.parsedValue.size() + 1) * sizeof(wchar_t));
		}

		hrt += L"\r\n\t" + attr.second.hrt;
	}

	// %14
	::EventDataDescCreate(&descriptors[currDescr++], hrt.c_str(), (hrt.size() + 1) * sizeof(wchar_t));

	///---

	if ((status = ::EventWrite(registrationHandle, &ObjectChangeEvent,
										currDescr, &descriptors[0])) != ERROR_SUCCESS)
	{
		throw std::runtime_error(utils::FormatLastErrorA("EventWrites"));
	}

	::EventUnregister(registrationHandle);
}
