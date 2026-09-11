void CUser::incTotalHackUserTradeGold(unsigned long long gold)
{
    // ORIG：内联 64 位读+adc 累加（mov 0x8d1b0/0x8d1ac + add + adc），无 getter 调用。
    m_379.m_hackTradeGold += gold;
}