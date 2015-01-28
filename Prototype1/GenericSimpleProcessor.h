#pragma once

#include "EventProcessorBase.h"
#include <memory>
#include <vector>
#include "EventUtils.h"
#include <iterator>
#include <algorithm>

template <class ChangeClass, typename Parser>
	class CGenericSimpleEventProcessor : public CEventProcessorBase
	{
	public:
		virtual void Proceed() override
		{
			// processing in Add(...)
		}

		virtual void Add(EventUtils::RawEvent1& evt) override
		{
			EventUtils::ParsedEvent1 pe;
			pe.system = evt.system;
			pe.eventData = Parser::Parse(evt);

			EventUtils::DumpEvent(pe);

			auto new_change = std::make_shared<ChangeClass>(pe);
			m_objectChanges.push_back(new_change);
		}

		virtual void GetAndClearChanges(ChangeUtils::object_changes_t* objectChanges, time_t* timeToWait) override
		{
			for (auto& change : m_objectChanges)
			{
				objectChanges->push_back(change->GetObjectChange());
			}

			m_objectChanges.clear();
		}

		CGenericSimpleEventProcessor()
		{

		}

		virtual ~CGenericSimpleEventProcessor()
		{

		}

	private:

		typedef std::vector<std::shared_ptr<ChangeClass>> object_changes_vector_t;
		object_changes_vector_t m_objectChanges;
};
