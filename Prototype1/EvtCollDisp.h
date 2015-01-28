#pragma once

#include <set>
#include "EventCollector1.h"
#include "Config.h"
#include <memory>
#include "XMLQueryGenBase.h"

class CEvtCollDisp
{
private:
	std::set<std::shared_ptr<CEventCollector1>> m_collectors;

public:
	void Init(std::shared_ptr<IXMLQueryGen> xmlQueryGen,
			const Config::targets_t& targets,
			const std::wstring& bkmPath);

	void Start();
	void Stop();

	CEvtCollDisp();
	virtual ~CEvtCollDisp();
};

