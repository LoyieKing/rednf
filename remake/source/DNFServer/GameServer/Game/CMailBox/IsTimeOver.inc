// ORIG 0x085574e0 (W)：m_period <= nowSec
bool CMailBox::CMail::IsTimeOver(long nowSec) const { return m_period <= nowSec; }
