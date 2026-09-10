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
    if (user->get_charac_guildkey() == 0)
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
    int masterNameLen = 0;
    if (!packet.get_int(masterNameLen))
    {
        return LineFunc(
            0xa35b,
            __PRETTY_FUNCTION__,
            0, 0);
    }
    char masterName[30];
    if (!packet.get_str(masterName, 0x1e, masterNameLen))
    {
        return LineFunc(
            0xa35e,
            __PRETTY_FUNCTION__,
            0, 0);
    }
    GlobalData::s_guild_proxy_mgr->GetServerProxy(user->GetServerGroup())
        ->SendGuildMasterDelegate((unsigned int)user->get_charac_guildkey(),
                                  (unsigned int)user->get_charac_no(-1),
                                  masterNameLen, masterName);
    return 0;
