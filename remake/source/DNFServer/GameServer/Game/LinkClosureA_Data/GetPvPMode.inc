int CDataManager::GetPvPMode()
{
    return m_pvpChannel.m_field1c;      // [objdump 0x844d4b4 mov 0xb44c(%eax)]
