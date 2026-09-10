void Secu_GoldControl::AddImportMoney(unsigned int value)
{
    m_cur.m_import += value;
    m_cur.m_count[3]++;
    m_total.m_import += value;
    m_total.m_count[3]++;

    ((XNuclear::CHades*)m_pUser->getHades())->GoldPlus(
        value, (XNuclear::ENUM_GOLD_CONTROL_REASON)1);
