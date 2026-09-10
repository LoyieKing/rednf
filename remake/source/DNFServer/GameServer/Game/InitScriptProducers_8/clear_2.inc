void stVendingMachineInfo_t::clear()
{
    m_groupNum = 0;
    m_payType = -1;
    m_payAmount = 0;
    m_giveCount = 0;
    m_vecGive.clear();
    m_bonusCount = 0;
    m_vecBonus.clear();
