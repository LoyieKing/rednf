void CUserCharacInfo::SetCurCharacRevengeData(unsigned char count, unsigned char ticket)
{
    if (m_selected)
    {
    m_selected->m_villageAttackCount = count;
    m_selected->m_revengeDungeonCount = ticket;
    }
}