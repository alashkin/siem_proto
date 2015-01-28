#include "stdafx.h"
#include "ADSchemaReader.h"
#include <Windows.h>
#include <Iads.h>
#include <AdsHlp.h>
#include <AdsErr.h>
#include <atlbase.h>
#include <stdio.h>
#include <string>
#include <map>
#include <set>
#include <vector>
#include "utils.h"
#include <iostream>
#include "log.h"

CADSchemaReader::CADSchemaReader(const std::wstring& dc, const std::wstring& domainfqdn,
										const std::wstring& user, const std::wstring& password) 
												: m_dc(dc), m_domainfqdn(domainfqdn), m_user(user), m_password(password)
{
	_LOG(debug) << L"SchemaReader initialized with server:" << m_dc << L", domain: " << m_domainfqdn
																<< L" user: " << m_user << L"pwd: " << m_password;
}



CADSchemaReader::~CADSchemaReader()
{
}

void CADSchemaReader::Read(ADUtils::SchemaInfo* schemaInfo)
{
	_LOG(debug) << L"Reading multivalied attributes";

 	schemaInfo->multivalued_attributes.clear();

	HRESULT hr;
	
	hr = ::CoInitialize(0);
	boost::utils::scope_guard sg1 = boost::utils::make_guard([&](){ ::CoUninitialize(); });

	CComPtr<IADsOpenDSObject> spRoot;

	_LOG(debug) << "Opening LDAP provider";

	hr = ::ADsGetObject(L"LDAP:", __uuidof(spRoot), (void**)&spRoot);
	utils::CheckADsResult(hr);
	CComPtr<IDispatch> spSchemaDisp;

	std::wstring ldapPath = L"LDAP://";
	if (!m_dc.empty())
	{
		ldapPath += m_dc;
	}

	ldapPath += L"/CN = Schema, CN = Configuration";

	
	std::wstringstream ss(m_domainfqdn);
	std::wstring dc;
	while (std::getline(ss, dc, L'.')) 
	{
		ldapPath += L", DC=" + dc;
	}

	_LOG(debug) << L"Opening " << ldapPath;

	hr = spRoot->OpenDSObject(
		CComBSTR(ldapPath.c_str()),
		m_user.empty() ? 0 : CComBSTR(m_user.c_str()),
		m_password.empty() ? 0 : CComBSTR(m_password.c_str()),
		ADS_SECURE_AUTHENTICATION, &spSchemaDisp);

	utils::CheckADsResult(hr);

	CComPtr<IDirectorySearch> spSearch;

	hr = spSchemaDisp.QueryInterface(&spSearch);

	LPWSTR filterClass = L"(&(objectClass=attributeSchema)(isSingleValued=FALSE))";
	LPWSTR classAttributes[] = { L"lDAPDisplayName" };

	ADS_SEARCH_HANDLE hSearch;

	_LOG(debug) << L"Executing query: " << filterClass << L", looking for " << classAttributes;

	hr = spSearch->ExecuteSearch(filterClass, classAttributes, 
				sizeof(classAttributes) / sizeof(classAttributes[0]), &hSearch);
	boost::utils::scope_guard sg2 = boost::utils::make_guard([&](){ spSearch->CloseSearchHandle(hSearch); });

	std::set<std::wstring> setAttr;

	while (SUCCEEDED(hr = spSearch->GetNextRow(hSearch)))
	{
		if (hr == S_OK)
		{
			ADS_SEARCH_COLUMN attrName;
			hr = spSearch->GetColumn(hSearch, classAttributes[0], &attrName);
			utils::CheckADsResult(hr);
			boost::utils::scope_guard sg3 = boost::utils::make_guard([&](){ spSearch->FreeColumn(&attrName); });

			if (attrName.dwADsType == ADSTYPE_CASE_IGNORE_STRING)
			{
				_LOG(debug) << L"Loaded: " << attrName.pADsValues[0].CaseIgnoreString;
				schemaInfo->multivalued_attributes.insert(attrName.pADsValues[0].CaseIgnoreString);
			}
			else
			{
				throw std::runtime_error("lDAPDisplay name attribute not found");
			}
		}
		else if (hr == S_ADS_NOMORE_ROWS )
		{
			DWORD dwError = ERROR_SUCCESS;
			WCHAR szError[512];
			WCHAR szProvider[512];

			::ADsGetLastError(&dwError, szError, 512, szProvider, 512);
			if (dwError == ERROR_MORE_DATA)
			{
				continue; // have to fetch again
			}
			else if (dwError == ERROR_SUCCESS)
			{
				break;
			}
			else
			{
				throw std::runtime_error(utils::FormatADsResult(hr));
			}

		}
		else
		{
			throw std::runtime_error(utils::FormatADsResult(hr));
		}
	}

	_LOG(debug) << L"Total attributes loaded: " << schemaInfo->multivalued_attributes.size();
}