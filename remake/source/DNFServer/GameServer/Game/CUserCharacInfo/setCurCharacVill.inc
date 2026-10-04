void CUserCharacInfo::setCurCharacVill(char vill)
{
    if (!m_selected)
        return;
    m_selected->m_villPrev = m_selected->m_vill;
    if (G_GameWorld()->IsPVPChannel())
    {
        m_selected->m_villFixed = 2;
    }
    else
    {
        enableSaveCharacStat();
        m_selected->m_vill = (unsigned char)vill;
    }
}