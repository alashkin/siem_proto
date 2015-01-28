#pragma once

#include "SimpleChange.h"
#include "GenericSimpleProcessor.h"
#include "EventWithFlatEventDataParser.h"

typedef CGenericSimpleEventProcessor<CSimpleChange, CEventWithFlatEventDataParser> CObjectDeletedProcessor;
