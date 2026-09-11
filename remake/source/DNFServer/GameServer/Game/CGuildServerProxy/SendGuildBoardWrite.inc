void CGuildServerProxy::SendGuildBoardWrite(char* info, unsigned int boardNo,
                                            unsigned int charNo, int job,
                                            char growType, unsigned int guildKey)
{
    Packet_Guild_Request_Guild_Board_Write pkt;
    pkt.m_boardNo = boardNo;
    pkt.m_charNo = charNo;
    pkt.m_job = (unsigned char)job;
    pkt.m_growType = (unsigned char)growType;
    pkt.m_guildKey = guildKey;
    memcpy(pkt.m_info, info, 0x78);
    SendTcpPacket((char*)&pkt, pkt.packetSize);
}