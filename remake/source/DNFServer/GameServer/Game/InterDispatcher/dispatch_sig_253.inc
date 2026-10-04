int Inter_Send_Charac_no_from_monitor::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    Inter_Send_Charac_no_from_monitorPktView* pPkt = reinterpret_cast<Inter_Send_Charac_no_from_monitorPktView*>(data);
    PacketGuard packet;
    packet.put_header(1, 0x14b);
    if (pPkt->m_nameLen == 0)
        packet.put_byte(0);
    else
    {
        packet.put_byte(1);
        packet.put_int(pPkt->m_e);
        packet.put_int(pPkt->m_nameLen);
        packet.put_str(pPkt->m_name, pPkt->m_nameLen);
    }
    packet.finalize(true);
    user->Send(packet);
    return 0;
}