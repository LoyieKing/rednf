void CUserCharacInfo::setCurCharacVill(char vill)
{
    if (!m_selected)
        return;
    CUR->m_villPrev = CUR->m_vill;
    if (G_GameWorld()->IsPVPChannel())
    {
        CUR->m_villPvP = 2;
    }
    else
    {
        enableSaveCharacStat();
        CUR->m_vill = (unsigned char)vill;
    }
