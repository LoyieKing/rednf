void CUserCharacInfo::SetCurCharacAssaultMoney(int money)
{
    if (m_selected)
    {
    m_selected->m_assaultMoney = money;
    }
}