signed char CUserCharacInfo::getCurCharacVill() const
{
    if (!m_selected)
        return 0;
    if (G_GameWorld()->IsPvPVillageMapChannel())
        return 10;
    if (G_GameWorld()->IsPVPChannel())
        return CUR->m_villPvP;
    return CUR->m_vill;
