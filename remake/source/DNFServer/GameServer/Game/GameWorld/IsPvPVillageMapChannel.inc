bool GameWorld::IsPvPVillageMapChannel() const
{
    if ((unsigned int)(m_channelType - 0xe) <= 2) return true;
    return false;
}