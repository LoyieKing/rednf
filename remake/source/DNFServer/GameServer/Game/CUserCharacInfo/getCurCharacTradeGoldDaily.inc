unsigned int CUserCharacInfo::getCurCharacTradeGoldDaily()
{
    return m_selected ? reinterpret_cast<CReliablePerson*>(m_selected->m_reliablePerson)->GetTradeGoldDaily() : 0;
}
