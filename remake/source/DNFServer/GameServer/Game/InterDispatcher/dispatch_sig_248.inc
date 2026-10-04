// [b28d-lane2] verdict=DIFF ndiff=133（ORIG 0x084e2b94）：槽序/帧差(N82/P212/P221)+寄存器中转(N18a/N7)+混合残差；gap+6。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=194（ORIG 0x84e2b94）：P226 具名 bool/char 落槽+N18a 单段 regmove+P264 .o 重定位/立即数。
int Inter_ReqGameServerInfo::dispatch_sig(CUser* /*user*/, char* data, int /*len*/)
{
    Inter_ReqGameServerInfoPktView* pPkt = reinterpret_cast<Inter_ReqGameServerInfoPktView*>(data);
    // [P5·2026-09-11] P59/P222 守卫包裹形：ORIG 084e2bb8 `sete %al; test %al,%al;
    //   jne 084e2de0 <尾 return 0>`。早退形令 4.4 产 `je <正文>; mov $0x0,%eax; jmp <尾>`
    //   （多 2 条）；goto 共享尾标签形则多 `jmp`+`nop`（各试过，均劣）。
    //   探针 /tmp/P5/guard.cpp 四形实测：v3 `if (cond==7) { } else { 正文 } return 0;`
    //   逐条产 `cmp $0x7; sete %al; test %al,%al; jne <尾>` = ORIG 同形（v1/v2 产 setne+je，
    //   v4 早退产 je+jmp）。正文逐字未改，缩进保持原样（便于与报告逐句对照）。
    if (G_GameWorld()->GetChannelType() == 7)
    {
        // 通道类型 7：直落函数尾 return 0（ORIG 同）
    }
    else
    {

    // §P0：ORIG 对 m_a/m_b 全程重读（084e2bd6/084e2ce9 两次 m_a；084e2c32 内联 m_b），无槽往返；
    //   proxy 亦无具名局部（GetServerProxy 结果直接在 %eax 作 this）⇒ 链式调用。
    // ORIG 084e2bcb/084e2bcd 为正向 `if (IsIntegratedPvPBaseChannel())` 双分支（真分支在前）。
    if (G_GameWorld()->IsIntegratedPvPBaseChannel())
    {
        // [L9-C PATCH#2] ORIG 084e2bcb..084e2ce0 段：`movzbl 0xa(%eax),%eax;
        //   movzbl %al,%eax; test %eax,%eax; je case0; cmp $0x1,%eax; je case1; jmp 尾`
        //   = 值只读一次 + int 提升后 cmp 链 ⇒ 源形是 switch（P251/P261，同 MONITOR
        //   RegistGameServerAck 先例）；if/else-if 链恒退化为逐次重读 + 8 位 test/jne。
        switch ((int)(unsigned char)pPkt->m_cmd)
        {
        case 0:
            GlobalData::s_monitor_proxy_mgr
                ->GetServerProxy((ENUM_SERVER_GROUP)pPkt->m_serverGroup)
                ->SendGameServerInfo(
                    G_CEnvironment()->get_server_ip(),
                    (unsigned short)G_CEnvironment()->get_udp_port(),
                    (unsigned char)G_CEnvironment()->m_serverEnvir.m_serverGroup,
                    (unsigned char)G_CEnvironment()->get_channel_no());
            break;
        case 1:
            GlobalData::s_guild_proxy_mgr
                ->GetServerProxy((ENUM_SERVER_GROUP)pPkt->m_serverGroup)
                ->SendGameServerInfo(
                    G_CEnvironment()->get_server_ip(),
                    (unsigned short)G_CEnvironment()->get_udp_port(),
                    (unsigned char)G_CEnvironment()->m_serverEnvir.m_serverGroup,
                    (unsigned char)G_CEnvironment()->get_channel_no());
            break;
        }
    }
    else
    {
        // [L9-C PATCH#2] 同上（else 分支同型，ORIG 084e2d45..084e2d6a 一段）。
        switch ((int)(unsigned char)pPkt->m_cmd)
        {
        case 0:
            GlobalData::s_monitor_proxy_mgr
                ->GetServerProxy((ENUM_SERVER_GROUP)0)
                ->SendGameServerInfo(
                    G_CEnvironment()->get_server_ip(),
                    (unsigned short)G_CEnvironment()->get_udp_port(),
                    (unsigned char)G_CEnvironment()->m_serverEnvir.m_serverGroup,
                    (unsigned char)G_CEnvironment()->get_channel_no());
            break;
        case 1:
            GlobalData::s_guild_proxy_mgr
                ->GetServerProxy((ENUM_SERVER_GROUP)0)
                ->SendGameServerInfo(
                    G_CEnvironment()->get_server_ip(),
                    (unsigned short)G_CEnvironment()->get_udp_port(),
                    (unsigned char)G_CEnvironment()->m_serverEnvir.m_serverGroup,
                    (unsigned char)G_CEnvironment()->get_channel_no());
            break;
        }
    }
    }   // if (GetChannelType()==7) { } else { … }  包裹形（P59/P222）
    return 0;
}