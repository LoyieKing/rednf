bool CmmChannelProxy::SendChannelInfo()
{
    tagCS_UPDATE_CHANNEL_INFO pkt;
    // ORIG 0x081061b1：无 env 局部，每个字段各自 G_CEnvironment()（返回 %eax 直用）。
    pkt.m_gcNo = G_CEnvironment()->m_serverEnvir.m_gcNo;
    pkt.m_group = G_CEnvironment()->get_server_group();
    pkt.m_channelNo = G_CEnvironment()->m_serverEnvir.m_channelNo;
    pkt.m_maxUserNum = G_CEnvironment()->m_serverEnvir.m_maxUserNum;
    pkt.m_curUserNum = static_cast<int>(
        G_GameWorld()->m_UserInWorld.size());         // m_UserInWorld
    std::strcpy(pkt.m_serverIP,
                G_CEnvironment()->m_serverEnvir.m_serverIP);
    pkt.m_port = G_CEnvironment()->m_serverEnvir.m_tcpPort;

    m_udp.SendToServer(reinterpret_cast<char*>(&pkt), pkt.m_hdr.getSize(),
                       static_cast<unsigned short>(m_port), m_ip.c_str());
    return true;
