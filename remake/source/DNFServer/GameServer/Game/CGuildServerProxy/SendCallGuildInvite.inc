void CGuildServerProxy::SendCallGuildInvite(unsigned int caller, unsigned int guildKey,
                                            char* name)
{
    Packet_Guild_Call_Guild_Invite pkt;
    pkt.m_caller = caller;
    pkt.m_guildKey = guildKey;
    memcpy(pkt.m_name, name, 0x1d);
    SendPacket((char*)&pkt, pkt.packetSize);
}