CDimensionActivationEvent::~CDimensionActivationEvent()
{
    // ORIG D1 0x080eeaa0：恢复本类 vptr 后调 CEventBase::~CEventBase()；D0 再 operator delete。
}