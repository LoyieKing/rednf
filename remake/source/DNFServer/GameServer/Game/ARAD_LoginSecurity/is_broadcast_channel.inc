// is_broadcast_channel @ 0x08197684（与 is_all_pvp_channel 同语义实现）
int PVP_CHANNEL::is_broadcast_channel()
{
    int channelType = G_GameWorld()->GetChannelType();
    if (is_integrated_pvp_channel() || channelType == 7)
        return 1;
    return 0;
}