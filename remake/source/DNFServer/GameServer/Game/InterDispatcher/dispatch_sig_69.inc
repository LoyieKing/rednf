// [b28d-lane2] verdict=AE ndiff=0（ORIG 0x084ce548）：混合残差；gap0。
int Inter_UpdateLogout::dispatch_sig(CUser* /*user*/, char* data, int /*len*/)
{
    SIG_UPDATE_LOGOUT* logout = reinterpret_cast<SIG_UPDATE_LOGOUT*>(data);
    if (logout->m_accId == 0)
        return 0;
    Packet_Monitor_UDP_Logout_Complete packet;
    packet.m_dbid = logout->m_accId;
    packet.m_channel = (unsigned char)G_CEnvironment()->get_channel_no();
    // ORIG 0x084ce588：&packet 先物化 %ebx（lea -0x1b 跨 GetServerProxy 存活）；
    // proxy 结果免槽直链（技巧37，+0x68 mov %eax,(%esp)）
    // b28c-28：ORIG 该处为 `mov 0xe(%eax),%eax; mov %eax,%edx`（mem→scratch→
    // 目标两段），4.4 恒一步直落 %edx（N18a 反向：单段直落不可造回两段，
    // 加 (int) 中转实测无效，ndiff 仍 3）⇒ 保持裸形，记 N18a。
    GlobalData::s_monitor_proxy_mgr->GetServerProxy(
        (ENUM_SERVER_GROUP)logout->m_serverId)->SendTcpPacket(
            reinterpret_cast<char*>(&packet), 0xf);
    return 0;
}