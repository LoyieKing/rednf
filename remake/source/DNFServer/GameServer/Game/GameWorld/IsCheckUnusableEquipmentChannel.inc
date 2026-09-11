bool GameWorld::IsCheckUnusableEquipmentChannel()
{
    if (m_channelType == 0xe) return true;
    return false;
}