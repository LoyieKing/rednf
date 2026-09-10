int Inter_OnTimeEventRewardStartOnLogin::dispatch_sig(CUser* user, char* data, int len)
{
    int local_data;

    local_data = (int)data;
    if (user == 0)
        return 0;
    if (((OnTimeEventRewardStartOnLoginView*)local_data)->m_rewardNo != user->get_charac_no(-1))
        return 0;
    PacketGuard packet;
    packet.put_header(0, 0x105);
    packet.put_int(((OnTimeEventRewardStartOnLoginView*)local_data)->m_characNo);
    packet.finalize(true);
    user->Send(packet);
    return 0;
