// ORIG 0x816c066：仅基类构造 + vptr 置位 + movb $1,0x9（m_firstReset=true）。
// m_rewardStayTime/m_intervalStayTime 不清零（ORIG 无对应 movl）。
CEventStayTime::CEventStayTime()
    : CEventBase(), m_firstReset(true)
{
}