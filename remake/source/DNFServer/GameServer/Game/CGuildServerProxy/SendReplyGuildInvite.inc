void CGuildServerProxy::SendReplyGuildInvite(unsigned int charNo, unsigned char accept)
{
    Packet_Guild_Reply_Guild_Invite_From_Invited pkt;
    pkt.m_charNo = charNo;
    pkt.m_accept = accept;
    SendPacket((char*)&pkt, pkt.packetSize);
}