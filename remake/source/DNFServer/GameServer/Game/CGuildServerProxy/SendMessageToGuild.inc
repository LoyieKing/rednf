void CGuildServerProxy::SendMessageToGuild(unsigned int guildKey, int len, char* msg)
{
    Packet_Web_Notify_Message_To_Guild pkt;
    pkt.m_guildKey = guildKey;
    pkt.m_mode = 2;
    if (len < 0x65)
    {
        memcpy(pkt.m_payload, msg, len);
    }
    SendPacket((char*)&pkt, pkt.packetSize);
}