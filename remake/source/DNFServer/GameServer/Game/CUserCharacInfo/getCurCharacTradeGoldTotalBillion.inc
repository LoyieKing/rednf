unsigned int CUserCharacInfo::getCurCharacTradeGoldTotalBillion()
{
    return m_selected ? reinterpret_cast<CReliablePerson*>(m_selected->m_reliablePerson)->GetTradeGoldTotalBillion() : 0;
}
