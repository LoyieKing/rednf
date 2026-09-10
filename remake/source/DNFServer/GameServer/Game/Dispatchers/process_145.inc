int Dispatcher_GuildCargoHistory::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    // ORIG 0x081d0cb4：param+4 = check_error(...)；>0 返 0；<0 LineFunc(0x2c6d,err,0)；
    // ==0 构造 Packet_Guild_Cargo_History_Request，guildkey(+6)/characNo(+0xa)，
    // SendTcpPacket(pkt, *(ushort*)(pkt+2))，历史日志 reason=3 后返 0。
    stGuildCargoHistoryParam& out = static_cast<stGuildCargoHistoryParam&>(param);
    out.m_4 = check_error(user, msg, param);
    if (out.m_4 > 0)
    {
        return 0;
    }
    if (out.m_4 < 0)
    {
        return LineFunc(0x2c6d,
                        __PRETTY_FUNCTION__,
                        out.m_4, 0);
    }
    Packet_Guild_Cargo_History_Request pkt;   // ORIG W ctor 0x0822be8a（局部帧 -0x1e 起）
    // 帧偏移：-0x1c=pkt+0x2(ushort size)、-0x18=pkt+0x6(guildkey)、-0x14=pkt+0xa(characNo)
    *reinterpret_cast<int*>(reinterpret_cast<char*>(&pkt) + 6) =
        user->get_charac_guildkey();
    *reinterpret_cast<int*>(reinterpret_cast<char*>(&pkt) + 0xa) =
        user->getCurCharacNo();
    CGuildServerProxy* proxy =
        GlobalData::s_guild_proxy_mgr->GetServerProxy(user->GetServerGroup());
    proxy->SendTcpPacket(reinterpret_cast<char*>(&pkt),
                         *reinterpret_cast<unsigned short*>(reinterpret_cast<char*>(&pkt) + 2));
    user->m_historyLog.GuildCargoHistory(static_cast<ENUM_GUILD_CARGO_HISTORY_REASON>(3),
                                         user->get_charac_guildkey());
    return 0;
