unsigned int CUserCharacInfo::getCurCharacTradeGoldTotal()
{
    return m_selected ? reinterpret_cast<CReliablePerson*>(m_selected->m_reliablePerson)->GetTradeGoldTotal() : 0;
}
