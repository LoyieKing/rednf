void CUserCharacInfo::SetCurCharacRevengeData(unsigned char count, unsigned char ticket)
{
    if (m_selected)
    {
    CUR->m_revengeCount = count;
    CUR->m_revengeDungeonCount = ticket;
    }
}