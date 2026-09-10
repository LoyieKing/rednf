int PVP_CHANNEL::is_winpoint_pvp_channel()
{
    int channelType = G_GameWorld()->GetChannelType();
    switch (channelType)
    {
    case 4:
    case 8:
        return 1;
    default:
        return 0;
    }
