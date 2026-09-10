int Dispatcher_CheckJoinGuild::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    // [FL1 2026-09-10] ORIG 0x081d4bf6 直证（P0 槽律）：
    //   ① 序言首条 `mov 0x10(%ebp),%eax; mov %eax,-0xc(%ebp)` = 具名 `MSG_BASE* pMsg`
    //      （后续 `mov -0xc(%ebp),%eax; add $0x11,%eax` 在 strncpy 处从槽回读）；
    //   ② `call get_state; cmp $0x3,%eax; setne %al; test %al,%al; je` ⇒ **无槽**
    //      （P168a 负例：具名结果局部会使 4.4 落槽 + cmpl）⇒ 表达式直用；
    //   ③ `call get_charac_guildkey; test %eax,%eax; setne %al; test; je` ⇒ 无槽；
    //   ④ `call G_GameWorld; mov %eax,(%esp); call IsIntegratedPvPBaseChannel;
    //      test %al,%al; je` ⇒ world 与 isPvP 均无槽（链式内联）；
    //   ⑤ 仅 serverGroup（-0x10）与 &msg（-0xc）占槽。
    int serverGroup;
    MSG_BASE* pMsg = &msg;
    (void)param;
    if (user->get_state() != 3)
        return 0;
    if (user->get_charac_guildkey() != 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x15c, (unsigned char)0x12);
        return 0;
    }
    SIG_CHECK_JOIN_GUILD sig;
    memset(&sig, 0, 0x1c);
    if (G_GameWorld()->IsIntegratedPvPBaseChannel())
    {
        serverGroup = user->GetServerGroup();
    }
    else
    {
        // [FL1] 具名权威成员（CEnvironment.h：m_serverEnvir.m_serverGroup @+0x378）
        // ⇒ 4.4 折单条 `mov 0x378(%eax),%eax`（裸 char* +0x378 恒发 add+mov 两条）。
        serverGroup = G_CEnvironment()->m_serverEnvir.m_serverGroup;
    }
    strncpy(reinterpret_cast<char*>(&sig),
            reinterpret_cast<char*>(pMsg) + 0x11, 0x16);
    DB_CheckJoinGuild::makeRequest(user->GetUID(), &sig);
    return 0;
