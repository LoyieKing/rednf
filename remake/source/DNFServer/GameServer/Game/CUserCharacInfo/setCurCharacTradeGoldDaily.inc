void CUserCharacInfo::setCurCharacTradeGoldDaily(unsigned int gold)
{
    enableSaveCharacStat();
    reinterpret_cast<CReliablePerson*>(m_selected->m_reliablePerson)->SetTradeGoldDaily(gold);
}
