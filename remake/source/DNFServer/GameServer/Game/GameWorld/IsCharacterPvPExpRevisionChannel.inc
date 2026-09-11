bool GameWorld::IsCharacterPvPExpRevisionChannel() const  // ORIG 085dfa76
{
    if (m_channelType == 0xf) return true;
    return false;
}