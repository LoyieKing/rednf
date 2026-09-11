// ORIG 0x0811fd8a / 0x0811fdba：D1 重置 vptr 后调 CEventBase::~CEventBase；
// D0 调 D1 后 operator delete。空体 + 虚析构由编译器生成同形。
CLeadingChannelEvent::~CLeadingChannelEvent()
{
}
