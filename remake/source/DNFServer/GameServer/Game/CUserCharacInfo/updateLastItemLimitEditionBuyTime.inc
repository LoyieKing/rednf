void CUserCharacInfo::updateLastItemLimitEditionBuyTime(long t)
{
    if (m_selected)
    {
    m_selected->m_field122E = t;
    }
}