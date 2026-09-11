// ---- 构造 / 析构（ORIG 0x082a416c / 0x082a41e0）----
// ORIG：m_bTrading(+0x1c) 直接初始化（在数组成员构造循环之前清零，
// 声明顺序 m_bTrading 先于 m_slotsData）；m_bLocked(+0x1d) 在 ctor 体末尾赋值。
CTradeSpace::CTradeSpace()
    : m_bTrading(false)
{
    m_bLocked = 0;
}