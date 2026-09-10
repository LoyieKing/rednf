unsigned int CUserCharacInfo::getCurCharacTradeGoldTotal()
{
    return m_selected ? CUR->m_reliablePerson.GetTradeGoldTotal() : 0;
