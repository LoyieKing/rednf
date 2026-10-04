void CUserCharacInfo::DecreaseRevengeDungeonTicket()
{
    if (m_selected)
    {
    --m_selected->m_revengeDungeonCount;
    }
}