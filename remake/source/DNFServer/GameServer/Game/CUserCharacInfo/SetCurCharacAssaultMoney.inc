void CUserCharacInfo::SetCurCharacAssaultMoney(int money)
{
    if (m_selected)
    {
    m_selected->m_field12B1 = money;
    }
}