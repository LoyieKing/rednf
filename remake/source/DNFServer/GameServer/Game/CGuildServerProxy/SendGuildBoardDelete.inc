void CGuildServerProxy::SendGuildBoardDelete(unsigned int boardNo,
                                             unsigned int guildKey,
                                             unsigned int charNo)
{
    Packet_Guild_Request_Guild_Board_Delete pkt;
    // ORIG @846f0c1：0x10(guildKey)→+0x12、0x14(charNo)→+0xe（字段命名与参数交叉，按 ORIG）
    pkt.m_charNo = guildKey;
    pkt.m_guildKey = charNo;
    pkt.m_boardNo = boardNo;
    SendTcpPacket((char*)&pkt, pkt.packetSize);
}