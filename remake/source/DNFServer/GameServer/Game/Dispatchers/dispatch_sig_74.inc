int Dispatcher_CallGuildMembers::dispatch_sig(CUser* user, PacketBuf& packet)
{
    bool gate;
    int guildkey;
    unsigned int characNo;
    unsigned int serverGroup;
    CGuildServerProxy* proxy;

    (void)packet;
    if (user->get_state() > 2)
    {
        if (user->getCurCharacR() != 0)
            gate = false;
        else
            gate = true;
    }
    else
    {
        gate = true;
    }
    if (gate)
    {
        LineFunc(0x93c0,
                 __PRETTY_FUNCTION__,
                 0, 0);
        return 0;
    }
    // ORIG：get_charac_guildkey() 调用两次（条件判断 + 取值）
    if (user->get_charac_guildkey() != 0)
    {
        guildkey = user->get_charac_guildkey();
        characNo = (unsigned int)user->getCurCharacNo();
        serverGroup = (unsigned int)user->GetServerGroup();
        proxy = GlobalData::s_guild_proxy_mgr->GetServerProxy(
            (ENUM_SERVER_GROUP)serverGroup);
        proxy->SendPacketCallGuildMembers(characNo, (unsigned int)guildkey);
    }
    return 0;
