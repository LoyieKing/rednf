int Inter_ReqGameServerInfo::dispatch_sig(CUser* /*user*/, char* data, int /*len*/)
{
    Inter_ReqGameServerInfoPktView* pPkt = reinterpret_cast<Inter_ReqGameServerInfoPktView*>(data);
    if (G_GameWorld()->GetChannelType() == 7)
        return 0;

    // §P0：ORIG 对 m_a/m_b 全程重读（084e2bd6/084e2ce9 两次 m_a；084e2c32 内联 m_b），无槽往返；
    //   proxy 亦无具名局部（GetServerProxy 结果直接在 %eax 作 this）⇒ 链式调用。
    // ORIG 084e2bcb/084e2bcd 为正向 `if (IsIntegratedPvPBaseChannel())` 双分支（真分支在前）。
    if (G_GameWorld()->IsIntegratedPvPBaseChannel())
    {
        if (pPkt->m_a == '\0')
        {
            GlobalData::s_monitor_proxy_mgr
                ->GetServerProxy((ENUM_SERVER_GROUP)pPkt->m_b)
                ->SendGameServerInfo(
                    G_CEnvironment()->get_server_ip(),
                    (unsigned short)G_CEnvironment()->get_udp_port(),
                    (unsigned char)G_CEnvironment()->m_serverEnvir.m_serverGroup,
                    (unsigned char)G_CEnvironment()->get_channel_no());
        }
        else if (pPkt->m_a == '\x01')
        {
            GlobalData::s_guild_proxy_mgr
                ->GetServerProxy((ENUM_SERVER_GROUP)pPkt->m_b)
                ->SendGameServerInfo(
                    G_CEnvironment()->get_server_ip(),
                    (unsigned short)G_CEnvironment()->get_udp_port(),
                    (unsigned char)G_CEnvironment()->m_serverEnvir.m_serverGroup,
                    (unsigned char)G_CEnvironment()->get_channel_no());
        }
    }
    else
    {
        if (pPkt->m_a == '\0')
        {
            GlobalData::s_monitor_proxy_mgr
                ->GetServerProxy((ENUM_SERVER_GROUP)0)
                ->SendGameServerInfo(
                    G_CEnvironment()->get_server_ip(),
                    (unsigned short)G_CEnvironment()->get_udp_port(),
                    (unsigned char)G_CEnvironment()->m_serverEnvir.m_serverGroup,
                    (unsigned char)G_CEnvironment()->get_channel_no());
        }
        else if (pPkt->m_a == '\x01')
        {
            GlobalData::s_guild_proxy_mgr
                ->GetServerProxy((ENUM_SERVER_GROUP)0)
                ->SendGameServerInfo(
                    G_CEnvironment()->get_server_ip(),
                    (unsigned short)G_CEnvironment()->get_udp_port(),
                    (unsigned char)G_CEnvironment()->m_serverEnvir.m_serverGroup,
                    (unsigned char)G_CEnvironment()->get_channel_no());
        }
    }
    return 0;
