int CDataManager::GetPvPChannelType()
{
    // ORIG 0x085e68a8 W：mov 0xb440(%eax) → m_pvpChannel+0x10。
    return m_pvpChannel.m_channelType;
}