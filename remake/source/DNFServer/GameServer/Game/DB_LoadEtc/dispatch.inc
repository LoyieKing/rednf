bool DB_LoadEtc::dispatch(int userIdx, int slot, Stream* stream)
{
    // [b29-Lane21] P212 槽序反转律：ORIG sendOk@sig 更深（-0xd vs sig -0xc）
    // ⇒ sendOk 原码声明在 sig 之前（先声明→更深槽）。
    bool sendOk;
    if (!DBDispatcher::dispatch(userIdx, slot, stream))
    {
        return false;
    }

    // Stream::GetOutBuffer<SIG_LOAD_ETC>(param_3)（模板实例本 TU 隐式产出，
    // ORIG 对应 W 实例符号）。
    SIG_LOAD_ETC* sig = stream->GetOutBuffer<SIG_LOAD_ETC>();
    if (sig == 0)
    {
        return false;
    }

    GetUserRecord(sig);     // 返回值未被消费（ORIG call 后直接 continue）

    // ORIG 0x08403c69..0x08403c77 无栈槽多路比较链
    //   `test %eax,%eax; je; cmp $0x1; je; cmp $0xffffffff; je; jmp`
    // ⇒ 原码是 switch(GetUserPvp(sig))（P251/P261），if-else 链恒带状态槽。
    // case 叶块物理序 = 书写序（P324）：ORIG 块序为 case1(+0x8e) → case-1(+0xd8)
    // → case0(+0xea)，决策链由编译器生成（P46）。
    switch (GetUserPvp(sig))
    {
    case 1:
    {
        DB_CreateCharac createCharac;                 // C1 0x08400cc2
        createCharac.CreatePvP(sig->m_characNo);      // T 0x084026ec
        break;                                        // D1 0x08400cde + try 帧
    }
    case -1:
        sig->m_pvpResult.Clear();
        break;
    case 0:
    {
        // 内层同形：ORIG 0x08403cea `cmp $0xffffffff; je; cmp $0x1; je; jmp`（P251），
        // 块序 -1(+0x108) → 1(+0x11a)。
        switch (GetUserPvpBase(sig))
        {
        case -1:
            sig->m_pvpResult.Clear();
            break;
        case 1:
            sig->m_pvpResult.m_field10 = -1;    // ORIG sig+0x350（PvpResultType+0x10）
            break;
        }
        break;
    }
    }

    if (!LoadCharacItemStat(sig))
    {
        LogManager::logFormat(1, "DBThread.cpp",
                              __PRETTY_FUNCTION__,
                              0x14c6, "DB_LoadETC ERROR : LoadCharacItemStat Fail");
        return false;
    }

    if (!LoadLitmitItem(sig))
    {
        // ORIG 0x08403d7d..0x08403db8：ctor 前 `mov 0x4(%eax),%ebx` 缓存
        // sig->m_characNo，ctor 后才 mov %ebx,0x8(%esp) ⇒ 全表达式临时链（P227）。
        cMyTrace(__PRETTY_FUNCTION__, 0x14db, 0)(
            "LoadLimitItem middle fetch <charac:%d>", sig->m_characNo);
        return false;
    }

    if (!GetUserSkill(sig))
    {
        LogManager::logFormat(1, "DBThread.cpp",
                              __PRETTY_FUNCTION__,
                              0x14e3, "DB_LoadETC ERROR : GetUserSkill Fail");
        sig->m_field8 = 1;
    }

    // GetRepeatEvent(0x71)->IsEventing(NULL)（虚表槽由 CEventBase 声明自然落位）
    if (GlobalData::s_event_manager->GetRepeatEvent(0x71)->IsEventing((CUser*)0))
    {
        LoadConditionEventInfo(sig);
    }

    LoadProperDungeonClearCount(sig);
    LoadRedeemItem(sig);
    LoadPvPMissionList(sig);
    LoadTODUserState(sig);
    LoadVillageAttackInfo(sig);
    LoadAccountFirstLogin(sig, 0x191);
    LoadAccountFirstLogin(sig, 0x192);
    LoadAccountFirstLoginDaily(sig, 0x19c);
    LoadFairPvPScore(sig);

    if (!LoadItemMakingSkill(sig))
    {
        LogManager::logFormat(1, "DBThread.cpp",
                              __PRETTY_FUNCTION__,
                              0x1566, "DB_LoadETC ERROR : LoadItemMakingSkill Fail");
        return false;
    }
    // ORIG 0x08403f5e..0x08403f6a：LoadQuestShop 返回值**丢弃**（call 后直接 LoadTitleBook，
    // 无 test/je）⇒ 原码为语句调用；旧 `if (!LoadQuestShop(sig)) return false;` 是多余门禁
    // （真 bug：失败时 ORIG 继续后续 Load，我方提前返回 false）。
    LoadQuestShop(sig);

    LoadTitleBook(sig);
    LoadAchievement(sig);
    LoadOnceGiveAvengerTitle(sig);
    LoadAuraAvatarOption(sig);
    LoadCharacOption(userIdx, slot, sig);
    LoadBoosterGage(sig);
    LoadPcRoomPlayTimeInfo(sig);
    LoadUsedGiftFatigueInfo(sig);
    // attendance 视图实参 = SIG_LOAD_ETC+0x14a99（&gUnicodeBuffer 别名标注），
    // accId = *(uint*)sig。
    LoadAttendanceInfo(&sig->m_attendanceEvent, sig->m_accId);
    LoadComboSkillInfo(sig);
    LoadSeriaRoomAniDeco(sig);

    // ORIG 0x0840404c 段：`mov %al,-0xd(%ebp); movzbl -0xd(%ebp),%eax; xor $1`
    // ⇒ 具名 bool 局部物化返回值（P102/P158），随后失败路径 `mov $0,%eax`。
    sendOk = SendResult(userIdx, slot, sig);
    if (!sendOk)
    {
        return false;
    }

    WongWork::EventCommon::LoadCharacEventItem(
        sig->m_accId, sig->m_characNo, 0);
    return true;
