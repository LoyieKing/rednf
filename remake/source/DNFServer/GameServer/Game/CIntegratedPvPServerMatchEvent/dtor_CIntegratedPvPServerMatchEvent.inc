// ORIG 0x0811d454 / 0x0811d484：D1 重置 vptr 后调 CEventBase::~CEventBase；
// D0 调 D1 后 operator delete。空体 + 虚析构由编译器生成同形。
CIntegratedPvPServerMatchEvent::~CIntegratedPvPServerMatchEvent()
{
}