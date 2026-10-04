void CUserCharacInfo::updateLastItemLimitEditionBuyTime(long t)
{
    if (m_selected)
    {
    m_selected->m_lastItemLimitEditionBuyTime = t;
    }
}