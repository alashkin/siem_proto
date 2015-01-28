#pragma once

#include <boost/msm/back/state_machine.hpp>
#include <boost/msm/front/state_machine_def.hpp>
#include <boost/msm/front/functor_row.hpp>
#include <iostream>

/*
	Detected -> WaitingForEvent -> Completed
		NeedMoreEvents?      IsCompleted?
*/

struct add_event {};

struct check_timeout {};

using boost::msm::front::none;

class CChangeBase : public boost::msm::front::state_machine_def<CChangeBase >
{
public:
	CChangeBase() 
	{}

	struct Detected : public boost::msm::front::state<>
	{
		template <typename EVT, typename FSM>
		void on_entry(EVT&, FSM&)
		{
			std::cout << "on_entry::Detected" << std::endl;
		}
	};

	struct WaitingForEvent : public boost::msm::front::state<>
	{
		template <typename EVT, typename FSM>
		void on_entry(EVT&, FSM& )
		{
			std::cout << "on_entry::WaitingForEvent" << std::endl;
		}
	};
	struct Completed : public boost::msm::front::state<>
	{
		template <typename EVT, typename FSM>
		void on_entry(EVT&, FSM&)
		{
			std::cout << "on_entry::Completed" << std::endl;
		}
	};

	typedef boost::mpl::vector<Detected> initial_state;

	struct NeedMoreEvents
	{
		template <typename EVT, typename FSM, typename SourceState, typename TargetState>
		bool operator()(EVT& evt, FSM& fsm, SourceState, TargetState) const
		{
			std::cout << "NeedMoreEvents::()" << std::endl;
			return false;
		}
	};

	struct IsCompleted
	{
		template <typename EVT, typename FSM, typename SourceState, typename TargetState>
		bool operator()(EVT& , FSM& , SourceState, TargetState) const
		{
			std::cout << "IsCompleted::()" << std::endl;
			return true;
		}
	};

	struct IsTimeoutExpired
	{
		template <typename EVT, typename FSM, typename SourceState, typename TargetState>
		bool operator()(EVT&, FSM&, SourceState, TargetState) const
		{
			std::cout << "IsTimeoutExpired::()" << std::endl;
			return true;
		}
	};

	struct transition_table : public boost::mpl::vector<
								/* Start            Event         Target         Action     Guard*/
		boost::msm::front::Row < Detected       , none         , Completed      , none, IsCompleted      >,
		boost::msm::front::Row < Detected       , none         , WaitingForEvent, none, NeedMoreEvents   >,
		boost::msm::front::Row < WaitingForEvent, add_event    , WaitingForEvent, none, NeedMoreEvents   >,
		boost::msm::front::Row < WaitingForEvent, add_event    , Completed      , none, IsCompleted      >,
		boost::msm::front::Row < WaitingForEvent, check_timeout, Completed      , none, IsTimeoutExpired >
	> {};

};

typedef boost::msm::back::state_machine<CChangeBase > CChangeBaseFSM;