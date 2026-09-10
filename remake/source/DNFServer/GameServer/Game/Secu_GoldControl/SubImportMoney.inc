void Secu_GoldControl::SubImportMoney(unsigned int value)
{
    m_cur.m_importSub += value;
    m_cur.m_count[5]++;
    m_total.m_importSub += value;
    m_total.m_count[5]++;

    ((XNuclear::CHades*)m_pUser->getHades())->GoldMinus(
        value, (XNuclear::ENUM_GOLD_CONTROL_REASON)1);
