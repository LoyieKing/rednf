int Dispatcher_ApproveJoinGuild::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    MSG_APPROVE_JOIN_GUILD& mref = reinterpret_cast<MSG_APPROVE_JOIN_GUILD&>(msg);  // 裸偏移→具名字段（同偏移同宽度）
    if (user->get_state() != 3)
    {
        return 0;
    }
    if (!user->IsGuildMaster() && !user->IsSubGuildMaster())
    {
        return 0;
    }
    Packet_Approve_Join_Guild packet;   // local_26（ORIG 栈对象 0x16；头仅导出 ctor，字段经裸偏移）
    // [G1Dproc 2026-09-11] ORIG 0x081d511c 起（packet@-0x22）：三处写入为 **disp 直写**
    //   （`mov %eax,-0x18/-0x14/-0x10(%ebp)` = packet+0xa/+0xe/+0x12）⇒ 用头内 union 的
    //   `m_fields[3]` 具名视角（Packet_Approve_Join_Guild.h，sizeof/偏移不变），
    //   裸 char* cast 会生 lea+间接（P0/P82/P235）。
    packet.m_fields[0] = (int)user->get_charac_guildkey();   // +0x0a guildKey
    packet.m_fields[1] = (int)user->getCurCharacNo();        // +0x0e charNo
    packet.m_fields[2] = mref.m_characNo;                    // +0x12 no
    // ORIG 0x081d516x：`movzwl -0x20(%ebp),%eax`(packet+0x2 长度→%esi)、`lea -0x22(%ebp),%ebx`
    //   (&packet)、GetServerGroup→GetServerProxy 直链（无 proxy 具名槽，P0）；长度取
    //   packet+0x2 字段而非常量 0x16（原码传常量会少两条 movzwl）。
    GlobalData::s_guild_proxy_mgr->GetServerProxy(user->GetServerGroup())
        ->SendTcpPacket(reinterpret_cast<char*>(&packet),
                        (int)*(unsigned short*)((char*)&packet + 2));
    return 0;
