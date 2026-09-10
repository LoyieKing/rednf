int Dispatcher_CheckJoinGuild::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    char isPvP;
    int guildkey;
    GameWorld* world;
    SIG_CHECK_JOIN_GUILD sig;
    int serverGroup;
    int ret;

    (void)param;
    ret = user->get_state();
    if (ret != 3)
        return 0;
    guildkey = user->get_charac_guildkey();
    if (guildkey != 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x15c, (unsigned char)0x12);
        return 0;
    }
    memset(&sig, 0, 0x1c);
    world = G_GameWorld();
    isPvP = (char)world->IsIntegratedPvPBaseChannel();
    if (isPvP == '\0')
    {
        serverGroup = *(int*)(reinterpret_cast<char*>(G_CEnvironment()) + 0x378);
    }
    else
    {
        serverGroup = user->GetServerGroup();
    }
    strncpy(reinterpret_cast<char*>(&sig),
            reinterpret_cast<char*>(&msg) + 0x11, 0x16);
    DB_CheckJoinGuild::makeRequest(user->GetUID(), &sig);
    return 0;
