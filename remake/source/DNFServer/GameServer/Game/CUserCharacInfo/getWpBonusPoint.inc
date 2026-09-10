void CUserCharacInfo::getWpBonusPoint(WpBonusPointResult& out)
{
    if (m_selected)
        out = CUR->m_wpBonus;
