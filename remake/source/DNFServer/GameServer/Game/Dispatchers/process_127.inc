int Dispatcher_RefreshGuildInfo::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    (void)msg;
    (void)param;
    // [round5 A01] ORIG 0x081d4899 `setle`(=state<=2 则 return 0) + 0x081d48b7 `sete`
    //   （guildkey==0 则 return 0）：两条负极性独立 guard。
    if (user->get_state() <= 2)
    {
        return 0;
    }
    if (user->get_charac_guildkey() == 0)
    {
        return 0;
    }
    // [round5 A01] P0/P183：ORIG 0x081d4904..0x081d490f 三结果全在寄存器
    //   （%ebx guildkey / %esi characNo / receiver 链最后求值），无具名槽
    //   （旧码三局部 ⇒ 帧 0x10→0x28）。实参右到左：a1=guildkey 先求值。
    GlobalData::s_guild_proxy_mgr->GetServerProxy(user->GetServerGroup())
        ->SendRefreshGuildInfo((unsigned int)user->get_charac_no(-1),
                               (unsigned int)user->get_charac_guildkey());
    return 0;
