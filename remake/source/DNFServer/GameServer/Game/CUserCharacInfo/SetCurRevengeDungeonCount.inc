void CUserCharacInfo::SetCurRevengeDungeonCount(unsigned char count)
{
    if (m_selected)
    {
    m_selected->m_revengeDungeonCount = count;
    }
}