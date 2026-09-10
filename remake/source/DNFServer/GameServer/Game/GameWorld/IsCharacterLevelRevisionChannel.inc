bool GameWorld::IsCharacterLevelRevisionChannel() const  // ORIG 084ed128
{
    if ((unsigned int)(m_channelType - 0xe) <= 2) return true;  // ∈ [0xe, 0x10]
    return false;
