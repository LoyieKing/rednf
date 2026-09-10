int CDataManager::GetPvPTeamCount()
{
    return m_pvpChannel.m_field20;      // [objdump 0x8568ce6 mov 0xb450(%eax)]
}
