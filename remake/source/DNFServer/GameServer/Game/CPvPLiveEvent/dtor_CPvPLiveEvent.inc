// ---------------------------------------------------------------------------
// ORIG 0x08268ed4 / 0x08268f04：D1 重置 vptr 后调 CEventBase::~CEventBase；
// D0 调 D1 后 operator delete。空体 + 虚析构由编译器生成同形。
CPvPLiveEvent::~CPvPLiveEvent()
{
}