void Secu_GoldControl::AddMoney(unsigned int value)
{
    // ORIG：m_cur(+0x40).m_gold += value；m_cur.m_count[2]++；
    // m_total(+0x04).m_gold += value；m_total.m_count[2]++；hades GoldPlus。
    m_cur.m_gold += value;
    m_cur.m_count[2]++;
    m_total.m_gold += value;
    m_total.m_count[2]++;

    ((XNuclear::CHades*)m_pUser->getHades())->GoldPlus(
        value, (XNuclear::ENUM_GOLD_CONTROL_REASON)0);
}