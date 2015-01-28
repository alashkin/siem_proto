#include "stdafx.h"
#include "Dispatcher1.h"


CDispatcher1::CDispatcher1(Params& params) : m_params(params)
{
}


CDispatcher1::~CDispatcher1()
{
}


void CDispatcher1::Start()
{
	for (auto generator : m_params.changeGenerators)
	{
		generator->Start();
	}
}

void CDispatcher1::Stop()
{
	for (auto generator : m_params.changeGenerators)
	{
		generator->SendStop();
	}

	for (auto generator : m_params.changeGenerators)
	{
		generator->WaitStop();
	}
}
