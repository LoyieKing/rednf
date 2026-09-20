void CUserCharacInfo::IncrementCurCharacTradeGoldDaily(unsigned int gold)
{
    enableSaveCharacStat();
    reinterpret_cast<CReliablePerson*>(m_selected->m_reliablePerson)->IncrementTradeGold(gold);
}
