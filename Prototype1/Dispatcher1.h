#pragma once

#include <memory>
#include <vector>
#include "ChangeGenerator1.h"

class CDispatcher1
{
public:
	void Start();
	void Stop();
	
	struct Params
	{
		typedef std::vector<std::shared_ptr<CChangeGenerator1>> generators_vector_t;
		generators_vector_t changeGenerators;
	};

	CDispatcher1(Params& params);
	virtual ~CDispatcher1();

private:
	Params m_params;

};

