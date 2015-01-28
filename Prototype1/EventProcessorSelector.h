#pragma once

#include "EventProcessor2.h"
#include "EventProcessorBase.h"
#include <memory>
#include <set>
#include <vector>

class CEventProcessorSelector
{
public:
	struct Params
	{
		struct EIDProc
		{
			std::set<unsigned int> eventIDs;
			std::shared_ptr<CEventProcessorBase> proc;
		};

		typedef std::vector<EIDProc> EIDProc_vector_t;
		EIDProc_vector_t processors;
	};

	CEventProcessorSelector(Params& params);
	virtual ~CEventProcessorSelector();

	std::shared_ptr<CEventProcessorBase> GetProcessor(unsigned int eventID);

	typedef std::vector<std::shared_ptr<CEventProcessorBase>> processors_vector_t;
	processors_vector_t& GetProcessors();
private:

	static const unsigned int MaxEventID = 10000;
	std::shared_ptr<CEventProcessorBase> m_processors[MaxEventID];
	processors_vector_t m_processorsStorage;

};

