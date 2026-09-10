bool GameWorld::IsCharacterStatRevisionChannel() const  // ORIG W 086977da
{
    // sub $0xe / cmp $0x2 / ja：m_channelType ∈ [0xe,0x10] 返回 true
    if ((unsigned int)(m_channelType - 0xe) <= 2) return true;
    return false;
