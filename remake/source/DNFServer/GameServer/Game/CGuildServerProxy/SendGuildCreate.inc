void CGuildServerProxy::SendGuildCreate(unsigned int charNo, char* guildName,
                                        char* masterName)
{
    Packet_Request_Guild_Create pkt;
    pkt.m_charNo = charNo;
    // ORIG @846e323：memcpy 源是第二参 guildName（0x10(%ebp)）
    memcpy(pkt.m_guildName, guildName, 0x16);
    memset(&pkt.m_extra[0], 0, 0xd);   // m_extra@+0x25（线格式 0xd 清尾）
    SendTcpPacket((char*)&pkt, pkt.packetSize);
