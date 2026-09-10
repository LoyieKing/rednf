int Dispatcher_CallGuildAllMembers::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // [b23S3C] ORIG 0x081ffb1a 无 result 栈槽（P0 律）：bad = (state<=2 || characR==0)
    // 的 || 短路 + 早返在 4.4 恰产 mov$1/mov$0 常量岛 + test%al；二段 guildkey()!=0
    // 出 test;setne;test;je（probe /tmp/r1probe/p.cpp w3 逐行证实）。链式右值使
    // guildkey→esi、characNo→ebx 跨 call 缓存与 ORIG 同形。
    if (user->get_state() <= 2 || user->getCurCharacR() == 0)
        return LineFunc(0x93cd, __PRETTY_FUNCTION__, 0, 0);
    if (user->get_charac_guildkey() != 0)
    {
        GlobalData::s_guild_proxy_mgr->GetServerProxy(
            (ENUM_SERVER_GROUP)user->GetServerGroup())
            ->SendPacketCallGuildAllMembers(
                (unsigned int)user->getCurCharacNo(),
                (unsigned int)user->get_charac_guildkey());
    }
    return 0;
