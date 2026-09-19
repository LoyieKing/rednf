int BlueMarble::setDiceNumber()
{
    m_diceNumber = (unsigned int)m_pRands[0].randInt() %
                    (unsigned int)m_pScript->m_info.m_diceMax +
                m_pScript->m_info.m_diceMin;
#if !remake_verify
    return 0;   // DNF-FIX: 非 void 函数落底 —— g++ 13 在此发 ud2（ORIG 的 GCC 4.x 只是返回残留 eax）
#endif
}