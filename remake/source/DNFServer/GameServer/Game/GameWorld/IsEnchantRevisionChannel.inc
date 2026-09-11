bool GameWorld::IsEnchantRevisionChannel()
{
    switch (m_channelType)
    {
    case 7:
    case 0xe:
        return true;
    default:
        return false;
    }
}