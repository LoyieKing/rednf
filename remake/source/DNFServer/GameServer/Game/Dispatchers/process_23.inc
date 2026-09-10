int Dispatcher_GuildCargo::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    // ORIG 0x081d0ae2：param+4 = check_error(...)；>0 直接返 0；<0 LineFunc(0x2c26)；
    // ==0 构造 Packet_Guild_Cargo_Requst，填 guildkey(+6)/characNo(+0xa)，
    // SendTcpPacket(pkt, *(ushort*)(pkt+2))，写历史日志 reason=0 后返 0。
    stGuildCargoParam& out = static_cast<stGuildCargoParam&>(param);
    out.m_4 = check_error(user, msg, param);
    if (out.m_4 > 0)
    {
        return 0;
    }
    if (out.m_4 < 0)
    {
        return LineFunc(0x2c26,
                        __PRETTY_FUNCTION__,
                        out.m_4, user->get_acc_id());
    }
    Packet_Guild_Cargo_Requst pkt;   // ORIG W 0x0822be52（局部帧 -0x1e 起）
    // 帧偏移：-0x1c=pkt+0x2(ushort size)、-0x18=pkt+0x6(guildkey)、-0x14=pkt+0xa(characNo)
    *reinterpret_cast<int*>(reinterpret_cast<char*>(&pkt) + 6) =
        user->get_charac_guildkey();
    *reinterpret_cast<int*>(reinterpret_cast<char*>(&pkt) + 0xa) =
        user->getCurCharacNo();
    CGuildServerProxy* proxy =
        GlobalData::s_guild_proxy_mgr->GetServerProxy(user->GetServerGroup());
    proxy->SendTcpPacket(reinterpret_cast<char*>(&pkt),
                         *reinterpret_cast<unsigned short*>(reinterpret_cast<char*>(&pkt) + 2));
    user->m_historyLog.GuildCargo(static_cast<ENUM_GUILD_CARGO_HISTORY_REASON>(0),
                                  user->get_charac_guildkey());
    return 0;
