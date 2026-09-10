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
    // [G1Dproc 2026-09-11] ORIG 0x081d0d3x 直证（pkt@-0x1e）：guildkey 写 **pkt+0x0a**
    //   （`call get_charac_guildkey; mov %eax,-0x14(%ebp)`）、characNo 写 **pkt+0x0e**
    //   （`mov %eax,-0x10(%ebp)`）⇒ 原裸偏移 +6/+0xa **少 4**（语义错位）；
    //   且 ORIG 为 disp 直写 ⇒ 用共享头具名字段（Packet_Guild_Cargo_History_Request.h
    //   m_guildKey@+0xa / m_charNo@+0xe）。
    pkt.m_guildKey = user->get_charac_guildkey();
    pkt.m_charNo = user->getCurCharacNo();
    // ORIG 0x081d0d5x：`movzwl -0x1c(%ebp),%eax`(pkt+0x2 长度→%esi)、`lea -0x1e(%ebp),%ebx`
    //   (&pkt)、GetServerGroup→GetServerProxy 直链（**无** proxy 具名槽，P0）。
    GlobalData::s_guild_proxy_mgr->GetServerProxy(user->GetServerGroup())
        ->SendTcpPacket(reinterpret_cast<char*>(&pkt),
                        (int)*(unsigned short*)((char*)&pkt + 2));
    user->m_historyLog.GuildCargoHistory(static_cast<ENUM_GUILD_CARGO_HISTORY_REASON>(3),
                                         user->get_charac_guildkey());
    return 0;
