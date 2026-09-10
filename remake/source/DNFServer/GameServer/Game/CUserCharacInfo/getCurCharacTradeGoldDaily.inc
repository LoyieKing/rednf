unsigned int CUserCharacInfo::getCurCharacTradeGoldDaily()
{
    return m_selected ? CUR->m_reliablePerson.GetTradeGoldDaily() : 0;
