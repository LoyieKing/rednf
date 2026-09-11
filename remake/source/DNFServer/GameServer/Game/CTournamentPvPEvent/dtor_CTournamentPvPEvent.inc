// ORIG 0x082845e8 / 0x08284618：D1 重置 vptr 后调 CEventBase::~CEventBase；
// D0 调 D1 后 operator delete。空体 + 虚析构由编译器生成同形。
CTournamentPvPEvent::~CTournamentPvPEvent()
{
}