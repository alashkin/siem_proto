#pragma once
#include <WinEvt.h>
#include "utils.h"
#include <stdexcept>

class CEvtHandle
{
private:
	EVT_HANDLE m_hEvt;


public:
	CEvtHandle() : m_hEvt(nullptr) {};

	CEvtHandle(const EVT_HANDLE& hEvt)
	{
		m_hEvt = hEvt;
	}

	operator const EVT_HANDLE& () const
	{
		return m_hEvt;
	}

	const EVT_HANDLE& operator = (EVT_HANDLE hEvt) = delete;

	void CloseAndReassign(const EVT_HANDLE& hEvt)
	{
		if (m_hEvt != nullptr)
		{
			::EvtClose(m_hEvt);
		}

		m_hEvt = hEvt;
	}

	bool IsValid() const
	{
		return (m_hEvt != 0);
	}

	void Check(char* funcName) const
	{
		if (!IsValid())
		{
			throw std::runtime_error(utils::FormatLastErrorA(funcName));
		}
	}

	~CEvtHandle()
	{
		CloseAndReassign(nullptr);
	}
};
