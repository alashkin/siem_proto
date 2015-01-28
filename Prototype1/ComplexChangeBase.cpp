#include "stdafx.h"
#include "ComplexChangeBase.h"


CComplexChangeBase::CComplexChangeBase(const EventUtils::ParsedEvent1& evt) : CChangeBase(evt)
{
	m_objectChange->changeInfo.meta.CID = evt.eventData->at(EventUtils::Consts::EventData::CID);
}


CComplexChangeBase::~CComplexChangeBase()
{
}
