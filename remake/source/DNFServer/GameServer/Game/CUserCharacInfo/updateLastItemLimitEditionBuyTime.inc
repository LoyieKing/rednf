void CUserCharacInfo::updateLastItemLimitEditionBuyTime(long t)
{
    if (m_selected)
    {
    CUR->m_lastItemLimitEditionBuyTime = t;
    }
}