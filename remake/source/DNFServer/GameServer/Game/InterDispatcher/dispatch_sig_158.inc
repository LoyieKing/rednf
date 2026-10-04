int Inter_PvPChannelUserCount::dispatch_sig(CUser* /*user*/, char* data, int /*len*/)
{
    Inter_PvPChannelUserCountPktView* pPkt = reinterpret_cast<Inter_PvPChannelUserCountPktView*>(data);
    pPkt->m_userCount =
        g_schoolMgr.GetUserCount(
            pPkt->m_schoolNo);  // ORIG mov 0x12(%eax)→GetUserCount
    pPkt->m_channelNo =
        G_CEnvironment()->get_channel_no();
    pPkt->m_playCount =
        (unsigned int)CLeagueMatch::GetPlayCountPerOneTeam() & 0xff;
    pPkt->m_battleMode =
        (unsigned int)CLeagueMatch::GetBattleMode() & 0xff;
    // ORIG 0x084d82bc `mov -0xc(%ebp),%ebx`：ebx 从 pPkt 局部槽装载后跨 call
    // 传参（0x084d82de `mov %ebx,0x4(%esp)`），帧 push %ebx/sub 0x24。
    // P52：pPkt 实参值先物化并跨 GetServerProxy 调用存活（callee-saved 独占）；
    // 原 register asm("ebx") 绑定违反 P0/N96 已删，源侧以槽局部 pPkt 直传复现。
    GlobalData::s_monitor_proxy_mgr->GetServerProxy(
        (ENUM_SERVER_GROUP)pPkt->m_serverGroup
        )->SendTcpPacket(reinterpret_cast<char*>(pPkt), 0x2a);
    return 0;
}