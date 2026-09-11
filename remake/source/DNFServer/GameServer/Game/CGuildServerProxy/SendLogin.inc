void CGuildServerProxy::SendLogin(int guildKey, memberIdentificationNumber_t dbid,
                                  int channel, char* ip, unsigned char sex, char* ssn)
{
    Packet_Monitor_UDP_Login pkt;
    pkt.m_guildKey = guildKey;
    pkt.m_dbid = dbid;
    pkt.m_channel = (unsigned char)channel;
    if (ip != 0)
    {
        strncpy(pkt.m_ip, ip, 0x10);
    }
    else
    {
        LogManager::logFormat(
            1, "DF_GuildServerProxy.cpp",
            __PRETTY_FUNCTION__,
            0x533, "CGuildServerProxy::SendLogin! DB ID(%s)", NumberToString(dbid, 0));
    }
    pkt.m_sex = sex;
    memcpy(pkt.m_ssn, ssn, 6);
    SendTcpPacket((char*)&pkt, pkt.packetSize);
}