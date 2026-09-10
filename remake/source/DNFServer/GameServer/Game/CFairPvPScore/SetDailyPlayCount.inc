void CFairPvPScore::SetDailyPlayCount(int v)           // 0x8234f92
{
    // [B24D判词 N] 语义正确：if(v<0)v=0(ORIG jns 钳负)；m_dailyPlayCount=v；
    // diff=mov 0x8(%ebp),%eax 与 mov 0xc(%ebp),%edx 装载序互换（GCC4.1 vs 4.4），
    // 10/10 纯 lowering，语义等价。report NO 由 nm/objdump diff 兜底。
    if (v < 0)
        v = 0;                                         // ORIG jns/clear
    m_dailyPlayCount = v;
