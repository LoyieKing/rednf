int Dispatcher_Write_On_The_GuildBoard::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    // ORIG 0x081d31ee
    // msg+0xd：公会板标题文本区首字节（非 0 才有效）
    if (*(unsigned char*)((char*)&msg + 0xd) == 0)
    {
        *(int*)((char*)&param + 4) = 1;
        return 0;
    }

    // hasPreventString || hasSpecialCharacterExceptSpace → invalid（ORIG 两链 jne/je 合流）
    bool invalid;
    if (!G_CDataManager()->hasPreventString((const char*)((char*)&msg + 0xd)) &&
        !G_CDataManager()->hasSpecialCharacterExceptSpace((const char*)((char*)&msg + 0xd)))
    {
        invalid = false;
    }
    else
    {
        invalid = true;
    }
    if (invalid)
    {
        *(int*)((char*)&param + 4) = 1;
        return 0;
    }

    // 实参求值顺序按 ORIG：guildkey → growType(movsbl) → job → characNo → accId → GetServerGroup
    unsigned int guildkey = (unsigned int)user->get_charac_guildkey();
    char growType = (char)user->getCurCharacGrowType();
    int job = user->get_charac_job();
    unsigned int characNo = (unsigned int)user->getCurCharacNo();
    unsigned int accId = user->get_acc_id();
    ENUM_SERVER_GROUP group = (ENUM_SERVER_GROUP)user->GetServerGroup();
    CGuildServerProxy* proxy = GlobalData::s_guild_proxy_mgr->GetServerProxy(group);
    proxy->SendGuildBoardWrite((char*)((char*)&msg + 0xd), accId, characNo, job, growType,
                               guildkey);
    return 0;
