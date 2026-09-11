void CGuildServerProxy::SendGuildBoardOpen(unsigned int charNo, unsigned int guildKey,
                                           char boardType)
{
    Packet_Guild_Request_Guild_Board_Open pkt;
    pkt.m_charNo = charNo;
    pkt.m_guildKey = guildKey;
    pkt.m_boardType = (char)(boardType != 0);
    SendTcpPacket((char*)&pkt, pkt.packetSize);
}