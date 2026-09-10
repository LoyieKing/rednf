CExpDoubleEvent::~CExpDoubleEvent()
{
    // ORIG D1：恢复本类 vptr，再调 CEventBase::~CEventBase()（空）。
