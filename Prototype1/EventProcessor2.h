#pragma once
#include "EventUtils.h"
#include "ChangeUtils.h"
#include <vector>
#include <memory>
#include "EDBasedObjectChange2.h"
#include "EventProcessorBase.h"

class CEventProcessor2 : public CEventProcessorBase
{
public:
	struct Params
	{
		unsigned int wait_for_event_min;
		unsigned int wait_for_event_max;
		unsigned int max_event_tolerance;
	};

	virtual void Proceed() override;

	virtual void Add(EventUtils::RawEvent1& evt) override;
	virtual void GetAndClearChanges(ChangeUtils::object_changes_t* objectChanges, time_t* waitTime) override;

	CEventProcessor2(Params& params);
	virtual ~CEventProcessor2();


private:
	typedef std::vector<std::shared_ptr<CEDBasedObjectChange2>> object_changes_vector_t;
	object_changes_vector_t m_objectChanges;
	
	parsed_events_t m_events;

	Params m_params;
};

