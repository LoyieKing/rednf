void CUserCharacInfo::getWpBonusPoint(WpBonusPointResult& out)
{
    if (m_selected)
        out = m_selected->m_wpBonusPointResult;
}