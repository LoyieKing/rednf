void CUserCharacInfo::setCurCharacTradeGoldDaily(unsigned int gold)
{
    enableSaveCharacStat();
    CUR->m_reliablePerson.SetTradeGoldDaily(gold);
