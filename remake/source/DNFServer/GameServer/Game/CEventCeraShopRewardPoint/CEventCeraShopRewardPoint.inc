// ORIG 0x0816459a：基类构造 + vptr 置位 + 清 m_field10(+0x10) / m_fieldC(+0x0c)
// （GCC 自动按声明序：先 +0x10 后 +0x0c，与 ORIG movl $0,0x10 → movl $0,0xc 同形）。
CEventCeraShopRewardPoint::CEventCeraShopRewardPoint()
{
    m_field10 = 0;
    m_fieldC = 0;
}