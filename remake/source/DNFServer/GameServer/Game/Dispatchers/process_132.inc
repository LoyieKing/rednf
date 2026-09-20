// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x081d4e28 orig/ours=89/98 ndiff-blocks=7
//   帧差 sub $68 vs $64 + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: sub    $0x68,%esp | mov    0x10(%ebp),%eax | mov    %eax,-0xc(%ebp)
int Dispatcher_RequestJoinGuild::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    (void)param;
    MSG_BASE& m = msg;                       // ORIG -0xc 槽（0x081d4e2e，函数第一句）
    // [G1Dproc 2026-09-10] ORIG 0x081d4e28（0x14c）直证：
    //   0x081d4e2e `mov 0x10(%ebp),%eax; mov %eax,-0xc(%ebp)` ⇒ 第一句是具名 msg 引用槽（-0xc），
    //   其后所有 msg 访问经该槽重载（0x081d4ea7 `mov -0xc(%ebp),%eax; add $0x11,%eax`）。
    //   0x081d4e67/0x081d4e82：guildkey!=0 → SendCmdErrorPacket(0x15d, 0x12); return 0。
    if (user->get_state() != 3)
    {
        return 0;
    }
    if (user->get_charac_guildkey() != 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x15d, 0x12);
        return 0;
    }

    // [G1Dproc 2026-09-10] 载荷偏移 ORIG 直证（= W8LT_DBTask.cpp 的 DB_RequestJoinGuildInView
    //   权威视图）：+0x00 guild_name[0x16] ← msg+0x11、+0x18 charac_no、+0x1c m_id、
    //   +0x20 server_group、+0x24 born_year[2]、+0x27 memo[0x19] ← msg+0x2c。
    //   原 +0x24/+0x28/+0x2c/+0x30/+0x32 全部错 0xc。
    //   形态：三处 int 写入为 `mov %eax,-0x38/-0x34/-0x30(%ebp)`（0x081d4ecf/…edd/…efc/…f0c）
    //   ⇒ P0/P82：必须经栈对象具名成员访问（char* 取址生 lea+%ebx 缓存、跨 call 保活）。
    struct JoinGuildSigView
    {
        char m_guildName[0x16];       // +0x00 ← msg+0x11（0x16 字节）
        char m_pad16[2];              // +0x16..+0x17
        unsigned int m_characNo;      // +0x18
        unsigned int m_id;            // +0x1c
        unsigned int m_serverGroup;   // +0x20
        char m_bornYear[2];           // +0x24（memcpy 2 字节）
        char m_pad26;                 // +0x26
        char m_memo[0x19];            // +0x27 ← msg+0x2c（0x19 字节）
        char m_pad40[4];              // +0x40..+0x43（保持 sizeof 0x44）
    };
    union JoinGuildSigBuf
    {
        SIG_REQUEST_JOIN_GUILD sig;   // 栈 0x44（SigTypes_D.h m_pad[0x44]）
        JoinGuildSigView       view;
    };
    JoinGuildSigBuf sigBuf;
    memset(&sigBuf.sig, 0, 0x44);                                 // 0x081d4e8c
    // [G3 负结论 2026-09-11] ORIG 0x081d4ea7/0x081d4f34 为就地形 `mov -0xc(%ebp),%eax;
    //   add $0x11,%eax`；试写 `(char*)&m + 0x11`（及 +0x2c）后 fresh 复编读数**逐值不变**
    //   （仍 88/89，仍 `lea 0x11(%eax),%edx`）⇒ 4.4 恒折 lea→新寄存器（N-注册表死区）。
    MSG_REQUEST_JOIN_GUILD& req = *reinterpret_cast<MSG_REQUEST_JOIN_GUILD*>(&m);
    strncpy(sigBuf.view.m_guildName, req.m_guildName, 0x16);
    sigBuf.view.m_characNo = (unsigned int)user->getCurCharacNo();        // 0x081d4ecf
    sigBuf.view.m_id = (unsigned int)user->get_acc_id();                  // 0x081d4edd
    if (G_GameWorld()->IsIntegratedPvPBaseChannel())
    {
        sigBuf.view.m_serverGroup = (unsigned int)user->GetServerGroup();  // 0x081d4efc
    }
    else
    {
        sigBuf.view.m_serverGroup =
            static_cast<unsigned int>(G_CEnvironment()->m_serverEnvir.m_serverGroup);
    }
    memcpy(sigBuf.view.m_bornYear, user->GetSsnString(), 2);             // 0x081d4f26
    strncpy(sigBuf.view.m_memo, req.m_memo, 0x19);

    DB_RequestJoinGuild::makeRequest(user->GetUID(), &sigBuf.sig);      // 0x081d4f5f
    return 0;
}