// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x0820791c orig/ours=148/146 ndiff-blocks=16
//   帧差 sub $40 vs $58 + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: push   %esi | push   %ebx | sub    $0x40,%esp
int Dispatcher_CallGuildInvite::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x0820792a：无 user==0 防御（直接 get_state；state<=2 失败）。
    if (user->get_state() < 3 || user->getCurCharacR() == 0) {
        return LineFunc(0xa24f, __PRETTY_FUNCTION__, 0, 0);
    }
    int secuRet = GlobalData::s_pSecuProtectionField->Check(user, (SECURITY_PROTCTION)0xb);
    if (secuRet != 0) {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x9a, (unsigned char)(secuRet & 0xff));
        return 0;
    }
    int guildKey;  // ORIG 0x08207a07：不预清零（直接作 get_int 输出缓冲）
    if (!packet.get_int(guildKey)) {
        return LineFunc(0xa25d, __PRETTY_FUNCTION__, 0, 0);
    }
    char guildName[30];
    if (!packet.get_str(guildName, 0x1e, guildKey)) {
        return LineFunc(0xa261, __PRETTY_FUNCTION__, 0, 0);
    }
    if (user->get_charac_guildkey() == 0) {
        this->send_packet_guild_invite(user, (unsigned char)0x22);
        return 0;
    }
    if (!user->IsSubGuildMaster() && !user->IsGuildMaster()
        && user->getGuildMemberGrade() != 3) {
        this->send_packet_guild_invite(user, (unsigned char)0x24);
        return 0;
    }
    // [DispG1/57] ORIG 0x08207b02 `mov %eax,%esi` / 0x08207b0f `mov %eax,%ebx` 保活 +
    //   0x08207b2e 一次性压参 ⇒ P52：单条链式表达式（receiver 不落具名槽）。
    GlobalData::s_guild_proxy_mgr->GetServerProxy(user->GetServerGroup())
        ->SendCallGuildInvite((unsigned int)user->getCurCharacNo(),
                              (unsigned int)user->get_charac_guildkey(),
                              guildName);
    return 0;
}