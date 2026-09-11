void CUserCharacInfo::IncrementCurCharacTradeGoldDaily(unsigned int gold)
{
    enableSaveCharacStat();
    CUR->m_reliablePerson.IncrementTradeGold(gold);
}