#pragma once
#include "ChangeUtils.h"

class CSIEMWriter
{
public:
	CSIEMWriter();
	virtual ~CSIEMWriter();

	void WriteEvent(const ChangeUtils::ObjectChange& objectChange);
};

