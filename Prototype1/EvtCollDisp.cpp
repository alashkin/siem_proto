#include "stdafx.h"
#include "EvtCollDisp.h"
#include "BkmConfig.h"

CEvtCollDisp::CEvtCollDisp()
{
}


CEvtCollDisp::~CEvtCollDisp()
{
}

void CEvtCollDisp::Init(std::shared_ptr<IXMLQueryGen> xmlQueryGen,
	const Config::targets_t& targets,
	const std::wstring& bkmPath)
{
	CBkmConfig bkmConfig;
	bkmConfig.Init(bkmPath);

	for (auto i : targets)
	{
		auto evtColl = std::make_shared<CEventCollector1>();
		evtColl->Init(i, bkmConfig.GetBkmStorage(i.server), xmlQueryGen);

		m_collectors.insert(evtColl);
	}
}

void CEvtCollDisp::Start()
{
	for (auto i : m_collectors )
	{
		i->Start();
	}
}

void CEvtCollDisp::Stop()
{
	for (auto i : m_collectors)
	{
		i->SendStop();
	}

	for (auto i : m_collectors)
	{
		i->WaitStop();
	}
}
