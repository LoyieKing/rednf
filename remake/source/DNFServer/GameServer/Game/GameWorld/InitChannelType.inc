bool GameWorld::InitChannelType()
{
    m_channelType = _GetChannelType();
    if (m_channelType == 0x13) return false;
    return true;
}