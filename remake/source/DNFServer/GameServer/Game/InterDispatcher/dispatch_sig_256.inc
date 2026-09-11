int Inter_SendTimeSyncLogin::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user == 0)
        return 0x53d4;
    Inter_SendTimeSyncLoginPktView* pPkt = reinterpret_cast<Inter_SendTimeSyncLoginPktView*>(data);
    PacketGuard packet;
    packet.put_header(0, 0x12c);
    packet.put_short((short)pPkt->m_12);
    packet.put_short((short)pPkt->m_14);
    packet.finalize(true);
    user->Send(packet);
    return 0;
}