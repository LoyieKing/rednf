int Inter_GuildExpBookDelete::dispatch_sig(CUser* /*user*/, char* data, int /*len*/)
{
    Inter_GuildExpBookDeletePktView* pPkt = reinterpret_cast<Inter_GuildExpBookDeletePktView*>(data);

    CUser* target = G_CGameManager()->getUser(
        pPkt->m_a, 2);
    if (target == 0)
    {   // 目标离线 → DB 删除 + monitor NoCache 广播
        DB_GuildExpBookDelete::makeRequest(
            pPkt->m_e, 1,
            (ENUM_SERVER_GROUP)pPkt->m_16);
        Packet_No_Cache packet;
        packet.m_dbid = pPkt->m_16;        // +0x0a
        packet.m_serverGroup = pPkt->m_12; // +0x0e
        CMonitorServerProxy* proxy =
            GlobalData::s_monitor_proxy_mgr->GetServerProxy(
                (ENUM_SERVER_GROUP)pPkt->m_16);
        proxy->SendPacket(reinterpret_cast<char*>(&packet), packet.packetSize);
    }
    else if (target->getCurCharacR() == 0)
    {   // 在线但未选角色 → 同上
        DB_GuildExpBookDelete::makeRequest(
            pPkt->m_e, 1,
            (ENUM_SERVER_GROUP)pPkt->m_16);
        Packet_No_Cache packet;
        packet.m_dbid = pPkt->m_16;        // +0x0a
        packet.m_serverGroup = pPkt->m_12; // +0x0e
        CMonitorServerProxy* proxy =
            GlobalData::s_monitor_proxy_mgr->GetServerProxy(
                (ENUM_SERVER_GROUP)pPkt->m_16);
        proxy->SendPacket(reinterpret_cast<char*>(&packet), packet.packetSize);
    }
    else
    {   // 目标可用 → DB 记录后现场删除
        DB_GuildExpBookDelete::makeRequest(
            pPkt->m_e, 0,
            (ENUM_SERVER_GROUP)pPkt->m_16);
        target->deleteGuildExpBook();
    }
    return 0;
