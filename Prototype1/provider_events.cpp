// provider_events.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"


#include <windows.h>
#include <stdio.h>
#include <evntprov.h>
#include "provider.h"  // Generated from manifest

#define SUNDAY     0X1
#define MONDAY     0X2
#define TUESDAY    0X4
#define WEDNESDAY  0X8
#define THURSDAY   0X10
#define FRIDAY     0X20
#define SATURDAY   0X40

enum TRANSFER_TYPE {
	Download = 1,
	Upload,
	UploadReply
};

#define MAX_NAMEDVALUES          5  // Maximum array size
#define MAX_PAYLOAD_DESCRIPTORS  14 + (2 * MAX_NAMEDVALUES)

typedef struct _namedvalue {
	LPWSTR name;
	USHORT value;
} NAMEDVALUE, *PNAMEDVALUE;

void wmain(void)
{
	DWORD status = ERROR_SUCCESS;
	REGHANDLE RegistrationHandle = NULL;
	EVENT_DATA_DESCRIPTOR Descriptors[MAX_PAYLOAD_DESCRIPTORS];
	DWORD i = 0;

	// Data to load into event descriptors

	USHORT Scores[4] = { 45, 63, 21, 15 };
	USHORT ScoresCount = 4;
	ULONG pImage = (ULONG)&Scores;
	DWORD TransferType = Upload;
	DWORD Day = MONDAY | TUESDAY;
	LPWSTR Path = L"c:\\path\\folder\\file.ext";
	BYTE Cert[11] = { 0x2, 0x4, 0x8, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x0, 0x1 };
	PBYTE Guid = (PBYTE)&KAPAAI_PROTOTYPE1_PROVIDER;
	USHORT ArraySize = MAX_NAMEDVALUES;
	BOOL IsLocal = TRUE;
	NAMEDVALUE NamedValues[MAX_NAMEDVALUES] = {
		{ L"Bill", 1 },
		{ L"Bob", 2 },
		{ L"William", 3 },
		{ L"Robert", 4 },
		{ L"", 5 }
	};

	status = EventRegister(
		&KAPAAI_PROTOTYPE1_PROVIDER,      // GUID that identifies the provider
		NULL,               // Callback not used
		NULL,               // Context noot used
		&RegistrationHandle // Used when calling EventWrite and EventUnregister
		);

	if (ERROR_SUCCESS != status)
	{
		wprintf(L"EventRegister failed with %lu\n", status);
		goto cleanup;
	}

	// Load the array of data descriptors for the TransferEvent event. 
	// Add the data to the array in the order of the <data> elements
	// defined in the event's template. 

	EventDataDescCreate(&Descriptors[i++], &pImage, sizeof(ULONG));
	EventDataDescCreate(&Descriptors[i++], &ScoresCount, sizeof(ScoresCount));
	EventDataDescCreate(&Descriptors[i++], Scores, sizeof(Scores));
	EventDataDescCreate(&Descriptors[i++], Guid, sizeof(GUID));
	EventDataDescCreate(&Descriptors[i++], Cert, sizeof(Cert));
	EventDataDescCreate(&Descriptors[i++], &IsLocal, sizeof(BOOL));
	EventDataDescCreate(&Descriptors[i++], Path, (ULONG)(wcslen(Path) + 1) * sizeof(WCHAR));
	EventDataDescCreate(&Descriptors[i++], &ArraySize, sizeof(USHORT));

	// If your event contains a structure, you should write each member
	// of the structure separately. If the structure contained integral data types
	// such as DWORDs and the data types were aligned on an 8-byte boundary, you 
	// could use the following call to write the structure, however, you are 
	// encouraged to write the members separately.
	//
	// EventDataDescCreate(&EvtData, struct, sizeof(struct));
	//
	// Because the array of structures in this example contains both strings 
	// and numbers, you must write each member of the structure separately.

	for (int j = 0; j < MAX_NAMEDVALUES; j++)
	{
		EventDataDescCreate(&Descriptors[i++], NamedValues[j].name, (ULONG)(wcslen(NamedValues[j].name) + 1) * sizeof(WCHAR));
		EventDataDescCreate(&Descriptors[i++], &(NamedValues[j].value), sizeof(USHORT));
	}

	EventDataDescCreate(&Descriptors[i++], &Day, sizeof(DWORD));
	EventDataDescCreate(&Descriptors[i++], &TransferType, sizeof(DWORD));

	// Write the event. You do not have to verify if your provider is enabled before
	// writing the event. ETW will write the event to any session that enabled
	// the provider. If no session enabled the provider, the event is not 
	// written. If you need to perform extra work to write an event that you
	// would not otherwise do, you may want to call the EventEnabled function
	// before performing the extra work. The EventEnabled function tells you if a
	// session has enabled your provider, so you know if you need to perform the 
	// extra work or not.
	wchar_t* attrName = L"TestAttributeName";
	wchar_t* attrOldValue = L"TestOldValue";
	wchar_t* attrNewValue = L"TestNewValue";
	USHORT attributesCount = 1;

	EventDataDescCreate(&Descriptors[i++], attrName, (ULONG)(wcslen(attrName) + 1) * sizeof(WCHAR));
	EventDataDescCreate(&Descriptors[i++], attrOldValue, (ULONG)(wcslen(attrOldValue) + 1) * sizeof(WCHAR));
	EventDataDescCreate(&Descriptors[i++], attrNewValue, (ULONG)(wcslen(attrNewValue) + 1) * sizeof(WCHAR));
	EventDataDescCreate(&Descriptors[i++], &attributesCount, sizeof(attributesCount));

	status = EventWrite(
		RegistrationHandle,              // From EventRegister
		&TransferEvent,                  // EVENT_DESCRIPTOR generated from the manifest
		(ULONG)MAX_PAYLOAD_DESCRIPTORS,  // Size of the array of EVENT_DATA_DESCRIPTORs
		&Descriptors[0]                  // Array of descriptors that contain the event data
		);

	if (status != ERROR_SUCCESS)
	{
		wprintf(L"EventWrite failed with 0x%x", status);
	}

cleanup:

	EventUnregister(RegistrationHandle);
}
