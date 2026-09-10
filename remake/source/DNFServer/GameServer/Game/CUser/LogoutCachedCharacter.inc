void CUser::LogoutCachedCharacter(unsigned char param)
{
    // 0x0864cb66 主分支（LogoutCachedCharacter.md）：货币日志 → 缓存角色内存 →
    // Guild/Monitor 代理 Logout → param==0 双开检测 UDP Logout → PC 房通知 →
    // 清 ontime 奖励 → HeroMission 存档。
    SendMoenyLog();
    unsigned char cached = (unsigned char)CacheCharacterMemory();

    // ORIG 0x0864cb66：Guild 段与 Monitor 段各自完整重新取参（P48 no-CSE），
    // 无 GetServerProxy 判空——直接解引用调用 SendLogout（P140/P171 照抄）。
    unsigned int memberKey = (unsigned int)get_charac_memberkey();
    unsigned int guildKey = (unsigned int)get_charac_guildkey();
    unsigned int characNo = (unsigned int)get_charac_no(-1);
    char* name = (char*)getCurCharacName();
    int channel = G_CEnvironment()->get_channel_no();
    unsigned int accId = get_acc_id();
    CGuildServerProxy* guildProxy =
        GlobalData::s_guild_proxy_mgr->GetServerProxy(GetServerGroup());
    guildProxy->SendLogout(accId, channel, name, characNo,
                           guildKey, memberKey, param);

    memberKey = (unsigned int)get_charac_memberkey();
    guildKey = (unsigned int)get_charac_guildkey();
    characNo = (unsigned int)get_charac_no(-1);
    name = (char*)getCurCharacName();
    channel = G_CEnvironment()->get_channel_no();
    accId = get_acc_id();
    CMonitorServerProxy* monProxy =
        GlobalData::s_monitor_proxy_mgr->GetServerProxy(GetServerGroup());
    monProxy->SendLogout(accId, channel, name, characNo,
                         guildKey, memberKey, (int)param, cached);

    if (param == 0)
    {
        CEnvironment* env = G_CEnvironment();
        int dcGroup = env->m_serverEnvir.m_serverGroup;   // §4.3 具名(env+0x378)
        int ch = G_CEnvironment()->get_channel_no();       // ORIG 二次取 env
        GlobalData::s_double_check_proxy->SendLogout(get_acc_id(), ch, dcGroup);
    }

    if (getCurCharacR() != 0)
        SendLogoutToPCRoomServer();       // ORIG T 0x0868170c

    delete_ontime_reward_list();          // ORIG T 0x0868c3bc
    // ORIG T 0x08166d38：ARAD::Singleton<HeroMissionEvent>::Get()->saveEventData(this, true)
    ARAD::Singleton<HeroMissionEvent>::Get()->saveEventData(this, true);
