int Inter_OnTimeEventRewardStartOnLogin::dispatch_sig(CUser* user, char* data, int len)
{
    Packet_GTM_OntimeEvent_RequestReward* pkt =
        reinterpret_cast<Packet_GTM_OntimeEvent_RequestReward*>(data);
    if (user == 0)
        return 0;
    if (pkt->m_fieldC != user->get_charac_no(-1))
        return 0;
    PacketGuard packet;
    packet.put_header(0, 0x105);
    packet.put_int(pkt->m_fieldA);
    packet.finalize(true);
    user->Send(packet);
    return 0;
}