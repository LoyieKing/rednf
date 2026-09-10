int Dispatcher_GuildJoinList::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    // ORIG 0x081d52a6：state==3 且会长/副会长以外 → SendCmdErrorPacket(0x161,0x24)；
    // 否则按渠道取 server group 并发起 DB_GuildJoinList 请求。
    if (user->get_state() != 3)
    {
        return 0;
    }
    if (!user->IsGuildMaster() && !user->IsSubGuildMaster())
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x161, 0x24);
        return 0;
    }

    // ORIG 分支：整合 PvP 基础渠道 → 用户 server group；否则环境 +0x378。
    int serverGroup;
    if (!G_GameWorld()->IsIntegratedPvPBaseChannel())
    {
        serverGroup =
            *reinterpret_cast<int*>(reinterpret_cast<char*>(G_CEnvironment()) + 0x378);
    }
    else
    {
        serverGroup = (int)user->GetServerGroup();
    }

    // ORIG 调用顺序：get_charac_guildkey(0x0822f46c) → GetUID(0x080c8c96)。
    int guildKey = user->get_charac_guildkey();
    int uid = (int)user->GetUID();
    DB_GuildJoinList::makeRequest(uid, (unsigned int)guildKey,
                                   (ENUM_SERVER_GROUP)serverGroup);
    return 0;
