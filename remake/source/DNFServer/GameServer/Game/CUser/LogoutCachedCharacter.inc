void CUser::LogoutCachedCharacter(unsigned char param)
{
    // 0x0864cb66 主分支（LogoutCachedCharacter.md）：货币日志 → 缓存角色内存 →
    // Guild/Monitor 代理 Logout → param==0 双开检测 UDP Logout → PC 房通知 →
    // 清 ontime 奖励 → HeroMission 存档。
    SendMoenyLog();
    unsigned char cached = (unsigned char)CacheCharacterMemory();

    // ORIG 0x0864cb66：Guild 段与 Monitor 段各自完整重新取参（P48 no-CSE），
    // 无 GetServerProxy 判空——直接解引用调用 SendLogout（P140/P171 照抄）。
    // b28d-GrpD P183/P329：ORIG 的第一轮实参只落 4 个自动临时槽（param -0x50、
    //   memberKey -0x4c、guildKey -0x48、characNo -0x44），name/channel/accId 及
    //   proxy **不落槽**（+0x06c→%edi、+0x079→%esi、+0x083→%ebx、+0x0a2 GetServerProxy
    //   的 %eax 直接作 this）；我方旧「7 个具名量」写法把 7 个值全落 -0x34..-0x1c
    //   再逐个回读 ⇒ 多 7×2 条 mov、帧 0x78 vs ORIG 0x7c。改成单条链式调用表达式。
    GlobalData::s_guild_proxy_mgr->GetServerProxy(GetServerGroup())
        ->SendLogout(get_acc_id(), G_CEnvironment()->get_channel_no(),
                     (char*)getCurCharacName(), (unsigned int)get_charac_no(-1),
                     (unsigned int)get_charac_guildkey(),
                     (unsigned int)get_charac_memberkey(), param);

    // 第二轮同形（ORIG +0x0d7 起）：ORIG 自动临时槽 -0x40(cached)/-0x3c(param)/
    //   -0x38/-0x34/-0x30，同样无我方具名量。
    GlobalData::s_monitor_proxy_mgr->GetServerProxy(GetServerGroup())
        ->SendLogout(get_acc_id(), G_CEnvironment()->get_channel_no(),
                     (char*)getCurCharacName(), (unsigned int)get_charac_no(-1),
                     (unsigned int)get_charac_guildkey(),
                     (unsigned int)get_charac_memberkey(), (int)param, cached);

    if (param == 0)
    {
        // ORIG +0x19a 段：`call G_CEnvironment; mov 0x378(%eax),%esi`（第 3 实参直接
        //   落 %esi）→ `call G_CEnvironment; call get_channel_no`（%ebx）→ get_acc_id
        //   （%eax）；**无 env / dcGroup / ch 具名槽**（P183/P329，我方旧形多 3 个槽）。
        GlobalData::s_double_check_proxy->SendLogout(
            get_acc_id(), G_CEnvironment()->get_channel_no(),
            G_CEnvironment()->m_serverEnvir.m_serverGroup);
    }

    if (getCurCharacR() != 0)
        SendLogoutToPCRoomServer();       // ORIG T 0x0868170c

    delete_ontime_reward_list();          // ORIG T 0x0868c3bc
    // ORIG T 0x08166d38：ARAD::Singleton<HeroMissionEvent>::Get()->saveEventData(this, true)
    ARAD::Singleton<HeroMissionEvent>::Get()->saveEventData(this, true);
