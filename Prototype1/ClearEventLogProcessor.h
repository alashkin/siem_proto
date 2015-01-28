#pragma once

#include "GenericSimpleProcessor.h"
#include "ClearEventLogEventParser.h"
#include "SimpleChange.h"

typedef CGenericSimpleEventProcessor<CSimpleChange, CClearEventLogEventParser> CEventLogClearedProcessor;





