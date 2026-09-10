void CGuildServerProxy::SendMemoToGuild(unsigned int guildKey, int charNo,
                                        int len, char* memo)
{
    Packet_Guild_Write_Guild_Member_Memo pkt;
    pkt.m_charNo = charNo;
    pkt.m_guildKey = guildKey;
    memcpy(pkt.m_memo, memo, len);
    SendPacket((char*)&pkt, pkt.packetSize);
