bool PVP_CHANNEL::is_all_pvp_channel()
{
    int channelType = G_GameWorld()->GetChannelType();
    if (is_integrated_pvp_channel() || channelType == 7)
        return true;
    return false;
