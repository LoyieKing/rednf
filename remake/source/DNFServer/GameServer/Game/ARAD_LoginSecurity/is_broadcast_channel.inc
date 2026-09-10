int PVP_CHANNEL::is_broadcast_channel()
{
    int channelType = G_GameWorld()->GetChannelType();
    if (is_integrated_pvp_channel() || channelType == 7)
        return 1;
    return 0;
