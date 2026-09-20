void CUserCharacInfo::resetBonusPoint()
{
    if (m_selected)
    {
    memset(&m_selected->m_wpBonusPointResult, 0, sizeof(m_selected->m_wpBonusPointResult));
    memset(&m_selected->m_expBonusPointResult, 0, sizeof(m_selected->m_expBonusPointResult));
    }
}