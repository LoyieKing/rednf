void CUserCharacInfo::setCurCharacVill(char vill)
{
    if (!m_selected)
        return;
    m_selected->m_field25 = m_selected->m_field22;
    if (G_GameWorld()->IsPVPChannel())
    {
        m_selected->m_field23 = 2;
    }
    else
    {
        enableSaveCharacStat();
        m_selected->m_field22 = (unsigned char)vill;
    }
}