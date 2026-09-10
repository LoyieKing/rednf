void CUser::setTotalHackUserTradeGold(unsigned long long gold)
{
    // ORIG 0x84ec940：mov this,%ecx; mov -0x8,%eax; mov -0x4,%edx; mov %eax,0x8d1ac(%ecx);
    // mov %edx,0x8d1b0(%ecx)。ll 成员直写= 4.4 同形（probe w2 实证）。
    m_379.m_hackTradeGold = gold;
