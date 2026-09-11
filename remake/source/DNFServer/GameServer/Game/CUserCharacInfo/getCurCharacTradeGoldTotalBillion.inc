unsigned int CUserCharacInfo::getCurCharacTradeGoldTotalBillion()
{
    return m_selected ? CUR->m_reliablePerson.GetTradeGoldTotalBillion() : 0;
}