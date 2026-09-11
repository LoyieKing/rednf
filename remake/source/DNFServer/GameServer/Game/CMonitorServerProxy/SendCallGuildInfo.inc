void CMonitorServerProxy::SendCallGuildInfo(unsigned int charNo, unsigned int uid,
                                            unsigned char count, unsigned int* guildKeys)
{
    Packet_Monitor_Call_Guild_Info pkt;
    pkt.m_charNo = charNo;
    pkt.m_uid = uid;
    pkt.m_count = count;
    for (int i = 0; i < (int)count; i++)
    {
        pkt.m_guildKeys[i] = guildKeys[i];
    }
    pkt.packetSize = (unsigned short)(count * 4 + 0x13);
    SendPacket((char*)&pkt, pkt.packetSize);
}