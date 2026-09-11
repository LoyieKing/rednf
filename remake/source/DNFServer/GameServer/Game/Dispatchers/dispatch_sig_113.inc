// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x082082d6 orig/ours=129/128 ndiff-blocks=18
//   帧差 sub $50 vs $68 + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: push   %esi | push   %ebx | sub    $0x50,%esp
int Dispatcher_GuildMasterDelegate::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // [round5 A01] ORIG 0x082082e9..0x0820830b 负极性短路**物化**：
    //   `cmp $2; jle L0; call CurCharacR; test; jne L1; L0: mov$1; jmp L2;
    //    L1: mov$0; L2: test %al,%al; je BODY; <LineFunc 0xa337>` ⇒
    //   唯一复现形 = 负极性短路 guard（P214①）；正极性 `if(A&&B){…}` 恒产 jle+jne 直跳。
    if (user->get_state() <= 2 || user->getCurCharacR() == 0)
    {
        return LineFunc(0xa337, __PRETTY_FUNCTION__, 0, 0);
    }
    // ORIG 0x0820..：guildKey/characNo/group/proxy 全程寄存器（ebx/esi/edx）跨调用，
    //   无具名栈槽（我方具名使帧 0x50→0x68、多 6 槽）；条件与取值各自重新调用 getter。
    //   例外：ORIG `call get_charac_guildkey; mov %eax,-0x10(%ebp); cmpl $0x0,-0x10(%ebp);
    //   jne <ok>` —— 该次结果**占具名槽 -0x10**（P0 有槽往返）⇒ 恢复具名局部。
    unsigned int guildKey = user->get_charac_guildkey();
    if (guildKey == 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x9e, 0x22);
        return 0;
    }
    // ORIG 0x0820837a `xor $1,%eax; test %al; je`（bool callee，无 setne）⇒ 裸 `!` 形（P248 档①）。
    if (!user->IsGuildMaster())
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x9e, 0x24);
        return 0;
    }
    int secu = GlobalData::s_pSecuProtectionField->Check(user, (SECURITY_PROTCTION)0xe);
    if (secu != 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x9e,
                                 (unsigned char)(secu & 0xff));
        return 0;
    }
    // [AttackA b28d-r7 翻正·P339/P331] masterName 声明前提（ORIG masterName@-0x2e 浅于
    // masterNameLen@-0x34）：原声明序使 4.4 落 -0x14/-0x32（反置）；对调后逐条复现
    // ORIG 0x082082d6 区槽序，clean 残差 12→0（129=129，verdict NEAR→AE）。
    char masterName[30];
    int masterNameLen = 0;
    if (!packet.get_int(masterNameLen))
    {
        return LineFunc(
            0xa35b,
            __PRETTY_FUNCTION__,
            0, 0);
    }
    if (!packet.get_str(masterName, 0x1e, masterNameLen))
    {
        return LineFunc(
            0xa35e,
            __PRETTY_FUNCTION__,
            0, 0);
    }
    GlobalData::s_guild_proxy_mgr->GetServerProxy(user->GetServerGroup())
        // [P4-p167] ORIG 复用守卫处缓存值（guildKey，见本函数上方 15351 行），
        //   只有 1 次 get_charac_guildkey 调用（p167: EXTRA 1）。
        ->SendGuildMasterDelegate(guildKey,
                                  (unsigned int)user->get_charac_no(-1),
                                  masterNameLen, masterName);
    return 0;
}