bool GameWorld::IsSchoolPvPChannel() const
{
    if ((unsigned int)(m_channelType - 9) <= 1) return true;
    return false;
}