#include "stdafx.h"
#include "EventProcessorSelector.h"
#include <stdexcept>
#include <boost/lexical_cast.hpp>

namespace
{
	void CheckID(unsigned int eid, unsigned int maxeid)
	{
		if (eid >= maxeid)
		{
			throw std::invalid_argument("EventID (" + boost::lexical_cast<std::string>(eid)+") > MaxEventID ("
				+ boost::lexical_cast<std::string>(maxeid));
		}
	}

}
CEventProcessorSelector::CEventProcessorSelector(Params& params) 
{
	for (auto& eid_proc : params.processors)
	{
		for (auto& eid: eid_proc.eventIDs)
		{
			CheckID(eid, MaxEventID);
			m_processors[eid] = eid_proc.proc;
		}

		m_processorsStorage.push_back(eid_proc.proc);
	}
}

CEventProcessorSelector::~CEventProcessorSelector()
{
}

std::shared_ptr<CEventProcessorBase> CEventProcessorSelector::GetProcessor(unsigned int eventID)
{
	CheckID(eventID, MaxEventID);

	return m_processors[eventID];
}

CEventProcessorSelector::processors_vector_t& CEventProcessorSelector::GetProcessors()
{
	return m_processorsStorage;
}