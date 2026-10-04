// [b28d-lane2] verdict=DIFF ndiff=99（ORIG 0x084e2f1e）：槽序/帧差(N82/P212/P221)+寄存器中转(N18a/N7)+混合残差；gap-7。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=93（ORIG 0x84e2f1e）：N18a 单段 regmove+N82/P112 帧槽平移+P30/N4 add-vs-lea。
int Inter_GuildExpBookDelete::dispatch_sig(CUser* /*user*/, char* data, int /*len*/)
{
    Inter_GuildExpBookDeletePktView* pPkt = reinterpret_cast<Inter_GuildExpBookDeletePktView*>(data);

    CUser* target = G_CGameManager()->getUser(
        pPkt->m_slotId, 2);
    if (target == 0)
    {   // 目标离线 → DB 删除 + monitor NoCache 广播
        DB_GuildExpBookDelete::makeRequest(
            pPkt->m_characNo, 1,
            (ENUM_SERVER_GROUP)pPkt->m_serverGroup);
        Packet_No_Cache packet;
        // ORIG 084e2f7f..084e2f8e（No_Cache 基址 -0x26）：先 `mov 0x16 → -0x18`（=pkt+0x0e
        //   m_serverGroup），后 `mov 0x12 → -0x1c`（=pkt+0x0a m_dbid）（Ghidra local_1c/local_20
        //   同证）。旧形两值互换（m_dbid←m_16、m_serverGroup←m_12）已按 ORIG 纠正。
        packet.m_serverGroup = pPkt->m_serverGroup;
        packet.m_dbid = pPkt->m_dbid;
        CMonitorServerProxy* proxy =
            GlobalData::s_monitor_proxy_mgr->GetServerProxy(
                (ENUM_SERVER_GROUP)pPkt->m_serverGroup);
        proxy->SendPacket(reinterpret_cast<char*>(&packet), packet.packetSize);
    }
    else if (target->getCurCharacR() == 0)
    {   // 在线但未选角色 → 同上
        DB_GuildExpBookDelete::makeRequest(
            pPkt->m_characNo, 1,
            (ENUM_SERVER_GROUP)pPkt->m_serverGroup);
        Packet_No_Cache packet;
        packet.m_serverGroup = pPkt->m_serverGroup;
        packet.m_dbid = pPkt->m_dbid;
        CMonitorServerProxy* proxy =
            GlobalData::s_monitor_proxy_mgr->GetServerProxy(
                (ENUM_SERVER_GROUP)pPkt->m_serverGroup);
        proxy->SendPacket(reinterpret_cast<char*>(&packet), packet.packetSize);
    }
    else
    {   // 目标可用 → DB 记录后现场删除
        DB_GuildExpBookDelete::makeRequest(
            pPkt->m_characNo, 0,
            (ENUM_SERVER_GROUP)pPkt->m_serverGroup);
        target->deleteGuildExpBook();
    }
    return 0;
}
