void Secu_GoldControl::SubMoney(unsigned int value)
{
    m_cur.m_sub += value;
    m_cur.m_count[4]++;
    m_total.m_sub += value;
    m_total.m_count[4]++;

    ((XNuclear::CHades*)m_pUser->getHades())->GoldMinus(
        value, (XNuclear::ENUM_GOLD_CONTROL_REASON)0);
}