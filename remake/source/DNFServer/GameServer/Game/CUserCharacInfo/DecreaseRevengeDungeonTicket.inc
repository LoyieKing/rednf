void CUserCharacInfo::DecreaseRevengeDungeonTicket()
{
    if (m_selected)
    {
    --CUR->m_revengeDungeonCount;
    }
}