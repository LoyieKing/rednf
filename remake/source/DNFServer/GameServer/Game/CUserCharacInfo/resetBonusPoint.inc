void CUserCharacInfo::resetBonusPoint()
{
    if (m_selected)
    {
    memset(&CUR->m_wpBonus, 0, sizeof(CUR->m_wpBonus));
    memset(&CUR->m_expBonus, 0, sizeof(CUR->m_expBonus));
    }
}