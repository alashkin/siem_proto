#include "stdafx.h"
#include "UACParser.h"
#include <boost/lexical_cast.hpp>
#include <iads.h>
#include "AttrParserUtils.h"

CUACParser::CUACParser()
{
}


CUACParser::~CUACParser()
{
}

void CUACParser::Parse(const std::wstring&, ChangeUtils::ObjectChange::AttributeChange* attrChange, const ChangeUtils::ObjectChange&)
{
	ChangeUtils::ObjectChange::attribute_values_t::iterator oldValue = attrChange->oldValues.begin();
	ChangeUtils::ObjectChange::attribute_values_t::reverse_iterator newValue = attrChange->newValues.rbegin();
	UINT32 uintOldValue, uintNewValue;

	if (oldValue != attrChange->oldValues.end())
	{
		uintOldValue = boost::lexical_cast<UINT32>(oldValue->rawValue);
		const_cast<std::wstring&>(oldValue->parsedValue) = ParseOneValue(uintOldValue);
	}

	if (newValue != attrChange->newValues.rend())
	{
		uintNewValue = boost::lexical_cast<UINT32>(newValue->rawValue);
		const_cast<std::wstring&>(newValue->parsedValue) = ParseOneValue(uintNewValue);
	}

	if ((oldValue != attrChange->oldValues.end()) && (newValue != attrChange->newValues.rend()))
	{
		UINT32 or_res = uintOldValue | uintNewValue;
		UINT32 added = uintOldValue ^ or_res;
		UINT32 removed = uintNewValue ^ or_res;

		if (added != 0)
		{
			attrChange->hrt = L"Controls added: " + ParseOneValue(added);
		}

		if (removed != 0)
		{
			attrChange->hrt += (attrChange->hrt.empty() ? L"" : L"; ") + std::wstring(L"Controls removed: ") + ParseOneValue(removed);
		}
	}
	else
	{
		attrChange->hrt = L"HRT composing error";
	}
}

std::wstring CUACParser::ParseOneValue(UINT32 value)
{
	static struct
	{
		ADS_USER_FLAG value;
		std::wstring text;
	} uacFlagsText[] =
	{
		{
			ADS_UF_SCRIPT,
			L"'The logon script is executed'"
		},
		{
			ADS_UF_ACCOUNTDISABLE,
			L"'The user account is disabled'"
		},
		{
			ADS_UF_HOMEDIR_REQUIRED,
			L"'The home folder is required'",
		},
		{
			ADS_UF_LOCKOUT,
			L"'The account is currently locked out'"
		},
		{
			ADS_UF_PASSWD_NOTREQD,
			L"'No password is required'"
		},
		{
			ADS_UF_PASSWD_CANT_CHANGE,
			L"'The user cannot change the password'"
		},
		{
			ADS_UF_ENCRYPTED_TEXT_PASSWORD_ALLOWED,
			L"'The user can send an encrypted password'"
		},
		{
			ADS_UF_TEMP_DUPLICATE_ACCOUNT,
			L"'This is an account for users whose primary account is in another domain'"
		},
		{
			ADS_UF_NORMAL_ACCOUNT,
			L"'This is a default account type that represents a typical user'"
		}, 
		{
			ADS_UF_INTERDOMAIN_TRUST_ACCOUNT,
			L"'This is a permit to trust an account for a system domain that trusts other domains'"
		},
		{
			ADS_UF_WORKSTATION_TRUST_ACCOUNT,
			L"'This is a computer account for a computer that is a member of this domain'"
		},
		{
			ADS_UF_SERVER_TRUST_ACCOUNT,
			L"'This is a computer account for a system backup domain controller that is a member of this domain'"
		},
		{
			ADS_UF_DONT_EXPIRE_PASSWD,
			L"'The password for this account will never expire'"
		},
		{
			ADS_UF_MNS_LOGON_ACCOUNT,
			L"'This is an MNS logon account'"
		},
		{
			ADS_UF_SMARTCARD_REQUIRED,
			L"'The user must log on using a smart card'"
		},
		{
			ADS_UF_TRUSTED_FOR_DELEGATION,
			L"'The account is trusted for Kerberos delegation'"
		},
		{
			ADS_UF_NOT_DELEGATED,
			L"'The security context of the user is not delegated to a service'"
		},
		{
			ADS_UF_USE_DES_KEY_ONLY,
			L"'This principal is restricted to use only Data Encryption Standard (DES) encryption types for keys'"
		},
		{
			ADS_UF_DONT_REQUIRE_PREAUTH,
			L"'This account does not require Kerberos pre-authentication for logon'"
		},
		{
			ADS_UF_PASSWORD_EXPIRED,
			L"'The user's password has expired'"
		},
		{
			ADS_UF_TRUSTED_TO_AUTHENTICATE_FOR_DELEGATION,
			L"'The account is enabled for delegation'"
		}
	};

	return AttrParserUtils::ConvertBitMask(value, uacFlagsText);
}

