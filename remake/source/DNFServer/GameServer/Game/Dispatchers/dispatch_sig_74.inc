int Dispatcher_CallGuildMembers::dispatch_sig(CUser* user, PacketBuf& packet)
{
    (void)packet;
    // ORIG 0x0820...：`state<=2 || CurCharacR==0` 单表达式直判（jle 短路物化）；
    //   guildkey/characNo/serverGroup/proxy 全为表达式临时（ORIG 仅 esi/ebx 寄存器，
    //   无栈槽）⇒ 不得用具名局部（P0/P48）。
    if (user->get_state() <= 2 || user->getCurCharacR() == 0)
    {
        // [G1-Dispatchers] ORIG 0x081ffa8d..0x081ffab1：LineFunc 后 `jmp 0x81ffb13`（直达
        // epilogue，**不经** 共享出口 0x81ffb0e 的 `mov $0x0,%eax`）⇒ 原码是
        // `return LineFunc(...);`（复用 LineFunc 的 %eax），不是我方 `LineFunc(); return 0;`。
        return LineFunc(0x93c0,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    // ORIG：get_charac_guildkey() 调用两次（条件判断 + 取值）
    if (user->get_charac_guildkey() != 0)
    {
        GlobalData::s_guild_proxy_mgr
            ->GetServerProxy((ENUM_SERVER_GROUP)user->GetServerGroup())
            ->SendPacketCallGuildMembers((unsigned int)user->getCurCharacNo(),
                                         (unsigned int)user->get_charac_guildkey());
    }
    return 0;
