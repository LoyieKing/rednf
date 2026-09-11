// ORIG 0x08268d38 / 0x08268d68：D1 重置 vptr 后调 CEventBase::~CEventBase；
// D0 调 D1 后 operator delete。空体 + 虚析构由编译器生成同形。
CPvPExpPenaltyEvent::~CPvPExpPenaltyEvent()
{
}