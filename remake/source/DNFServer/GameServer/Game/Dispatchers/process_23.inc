// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x081d0ae2 orig/ours=80/81 ndiff-blocks=14
//   帧差 sub $30 vs $34 + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: push   %esi | sub    $0x30,%esp | mov    0x14(%ebp),%eax
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
    // [G1Dproc 2026-09-11] ORIG 0x081d0b9x 直证（pkt@-0x1e）：guildkey 写 **pkt+0x0a**
    //   （`call get_charac_guildkey; mov %eax,-0x14(%ebp)`）、characNo 写 **pkt+0x0e**
    //   （`mov %eax,-0x10(%ebp)`）⇒ 原裸偏移 +6/+0xa **少 4**（语义错位）；
    //   ORIG 为 disp 直写 ⇒ 用共享头具名字段（Packet_Guild_Cargo_Requst.h
    //   m_guildKey@+0xa / m_charNo@+0xe）。
    // [父泳道修正] 头内真实成员名（Packet_Guild_Cargo_Requst.h）：+0x0a = m_characNo、
    // +0x0e = m_field0e ⇒ 按 ORIG 偏移写（G1Dproc 原写 m_guildKey/m_charNo 不存在）。
    pkt.m_characNo = user->get_charac_guildkey();   // +0x0a（ORIG `mov %eax,-0x14(%ebp)`）
    pkt.m_field0e = user->getCurCharacNo();         // +0x0e（ORIG `mov %eax,-0x10(%ebp)`）
    // ORIG 0x081d0bcx：size(pkt+0x2)→%esi、&pkt→%ebx、proxy 直链（无具名槽，P0）。
    GlobalData::s_guild_proxy_mgr->GetServerProxy(user->GetServerGroup())
        ->SendTcpPacket(reinterpret_cast<char*>(&pkt),
                        (int)pkt.packetSize);
    user->m_historyLog.GuildCargo(static_cast<ENUM_GUILD_CARGO_HISTORY_REASON>(0),
                                  user->get_charac_guildkey());
    return 0;
}