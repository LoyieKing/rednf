int Inter_NoticeCreateGuildAgit::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    Inter_NoticeCreateGuildAgitPktView* pPkt = reinterpret_cast<Inter_NoticeCreateGuildAgitPktView*>(data);
    user->SetGuildAgitFlag(true);

    // ORIG 0x084daa2a：pPkt 槽 -0xc 入口即物化；m_12 比较。
    if (pPkt->m_12 == user->get_charac_no(-1))
    {   // 目标即本人 → 成功回执
        PacketGuard packet;
        packet.put_header(1, 0xe7);
        packet.put_byte(1);
        packet.finalize(true);
        user->Send(packet);
    }

    PacketGuard notify;
    notify.put_header(0, 0xbd);
    notify.finalize(true);
    user->Send(notify);

    // b28c-l23 P0 清绑：ORIG 0x084daac1.. guildkey 两段 `mov %eax,%ebx` 跨 call 钉形，
    // proxy 结果 %eax 免槽直落 (%esp)。绑定形违 P0 已删，裸 register 保留（N96/N18a）。
    register unsigned int guildKey = user->get_charac_guildkey();
    GlobalData::s_guild_proxy_mgr->GetServerProxy(
        user->GetServerGroup())->ChangeGuildAgitFlagInProxy(guildKey, 1);
    return 0;
