// ---- CParty::dungeon_start（ORIG T 0x85a0954）----
// 返回：0=成功；1=SelectDungeon/封印门/血宫置态失败；0xe=等级不足；
// 0x13=未通关该难度或难度倒退；0x15=dungeon 查表失败；8=EPLP 状态异常；
// 其余为 GetDungeonIndex_isTOD/checkInoutConditionDungeon 透传。
// b28d-solo-CParty: N 封顶 —— N82 帧 0x20c/0x27c + P0 指针缓存；1715 vs 1662
int CParty::dungeon_start(int dungeonIdx, char type, ENUM_DUNGEON_TYPE dtype)
{
    // b28d-49 判词：N 封顶（N82 帧/槽）。本波已补 7 处 P167 MISSING 调用
    //（get_member_count/get_min_level×2/get_charac_level/get_standard_level/
    // GetInstanceDungeonEntranceLog+IncrementDungeonEntrance/SetWeekendEvent），
    // 现仅 EXTRA memset（P78/P116 内联差）。@0x85a0954
    // P183：ORIG 本函数 bf/bd 每使用点重取址（无引用缓存槽）⇒ 宏直用。
#define bf (*reinterpret_cast<CBattle_Field*>(&m_battleField))
#define bd (m_battleData)

    resetMemberUseCoin();
    m_firstMapClear = 0;                                   // +0x324
    ClearMapHitCount();
    m_secu.startDungeon();

    int todApcIdx = -1;                                    // -0x78
    int err = GetDungeonIndex_isTOD(dungeonIdx, todApcIdx);  // -0x74
    if (err != 0)
        return err;

    CDungeon* dungeon = G_CDataManager()->find_dungeon(dungeonIdx);   // -0x70
    if (dungeon == 0)
        return 0x15;

    PacketGuard packet;                                    // -0x84
    int partyType = m_battleField.m_bloodState;                           // -0x6c ← +0xcd8
    SetPartyMemberCoinLimit(get_member_count() *
                            dungeon->GetPartyMemberCoinLimit());
    m_traceMobDieHack.setDungeonDiff(type);

    if (GetEPLPState() != 0)
    {
        for (int i = 0; i <= 3; ++i)
        {
            if (!_checkValidUser(i))
                continue;
            WongWork::CHackAnalyzer* hack = m_member[i].m_pUser->getHackAnalyzer();
            hack->addServerHackCnt(m_member[i].m_pUser,
                                   (WongWork::ENUM_HACKTYPE)0x195, 1, 0, 0);
        }
        return 8;
    }

    // b28d-49 @0x85a0b17：ORIG 三调 get_min_level（85a0b26 条件 / 85a0b46 then /
    // 85a0b56 else），条件序 = IsLightServer 先、`get_min_level() > 1` 后（P157 逐点重调，
    // 不缓存）。原「先赋初值再条件重调」只有 2 次调用（P167 MISSING -1）。
    int minLevel;
    if (IsLightServer() != 0 && dungeon->get_min_level() > 1)
        minLevel = dungeon->get_min_level();
    else
        minLevel = dungeon->get_min_level();

    if (m_manager->IsGameMasterMode() != 1 &&
        minLevel > m_manager->get_charac_level())
        return 0xe;

    // [R13a14 2026-09-12 ④ 修复] 原写 `if (!checkDungeonDifficulty(...))` 与 ORIG 相反：
    //   ORIG 0x085a0bb8 `call checkDungeonDifficulty` → 0x085a0bbd `test %al,%al` →
    //   0x085a0bbf `je 0x85a0c1c`（f()==0 跳过本块）⇒ 原码是 **正条件**（ORIG 真时进入
    //   clearedDiff 检查、可能 return 0x13）。我方旧形 `xor $1; test; je`（f()==1 跳过）
    //   使两侧可观察行为相反（0x084a9aba `xor $0x1,%eax` 我方站点）。
    //   两侧 checkDungeonDifficulty 实现已逐条比对一致（ORIG 0x085be932 49 条 vs 我方 .inc）。
    if (checkDungeonDifficulty(type, dungeon))
    {
        char clearedDiff =
            m_manager->getDungeonClearRefR()->getClearedDungeonDiff(
                (short)dungeonIdx);                        // -0x51
        if (clearedDiff == 0)
            return 0x13;
        if ((clearedDiff < (char)type) && ((char)type != 4))
            return 0x13;
    }

    int questStage = 0;                                    // -0x64
    for (int i = 0; i <= 3; ++i)
    {
        if (!_checkValidUser(i))
            continue;
        if (partyType == 1)
            continue;
        questStage = m_member[i].m_pUser->CheckQuestMap(dungeon->get_index());
        if (questStage > 0)
            break;
    }

    SetUsedCoinCount(0);

    if (!CheckHaveHellPartyPassItem(dungeon, dtype))
        return 0;

    bool randomHell = false;                               // -0x85
    int inoutErr =
        checkInoutConditionDungeon(dungeon, (ENUM_DUNGEON_MODE)partyType,
                                   (ENUM_CMDPACKET)0x10);  // -0x60
    if (inoutErr != 0)
        return 0;

    bf.check_random_appear_hell_dungeon(dungeon, (ENUM_DUNGEON_MODE)partyType,
                                        dtype, randomHell);

    int memberCount = get_member_count();
    if (!bf.SelectDungeon(dungeonIdx, dungeon, (int)(char)type, dtype,
                          memberCount, m_hellWorldMap, randomHell, questStage))
        return 1;

    choose_random_buff((QuickParty::RandomBuffDungeon)bf.getRandomBuffDungeonType(0));

    // [2026-09-10 试形 P312/P78] ORIG 该点无 memset call（p167 EXTRA memset）⇒
    // `={0}` 让 4.4 内联 rep stos（OnFightVillageMonster/leave_user 同族实证）。
    char names[256] = {0};                                 // -0x1cc
    _getMemberNames(names);

    for (int i = 0; i <= 3; ++i)
    {
        if (!_checkValidUser(i))
            continue;
        reinterpret_cast<BestClearTime*>(m_member[i].m_pUser->GetCharacExpandData(
            (ENUM_CHARAC_EXPAND_TYPE)0x12))
            ->setNewRecord(false);

        int mode = partyType;                              // -0x70 复用槽
        char* names2 = _getMemberNames(names);
        const char* dunName = dungeon->GetDungeonName();
        m_member[i].m_pUser->m_historyLog.EnterDungeon(dunName, (int)(char)type,
                                                       names2, mode);
        m_member[i].m_pUser->resetDungeonClearHackCheck();

        if (m_member[i].m_pUser->getMoveSpace() == 0)
        {
            int firstEmpty = 0;                            // -0xb4
            SkillSlot* skillW = reinterpret_cast<SkillSlot*>(
                m_member[i].m_pUser->getCurCharacSkillW());
            int emptyCnt = skillW->checkskillSlotEmptyCount(firstEmpty);   // -0x44
            for (int k = 0; k < emptyCnt; ++k)
            {
                int set = m_member[i].m_pUser->SetETC(0x1a6, firstEmpty);
                WongWork::CHackAnalyzer* hack =
                    m_member[i].m_pUser->getHackAnalyzer();
                hack->addServerHackCnt(m_member[i].m_pUser,
                                       (WongWork::ENUM_HACKTYPE)0x1a6, 1,
                                       (unsigned int)set, 0);
            }

            int cntA = 0, cntB = 0, cntC = 0, cntD = 0, cntE = 0, cntF = 0;
            char growFirst = m_member[i].m_pUser->getCurCharFirstGrowType();
            int job = m_member[i].m_pUser->get_charac_job();
            CInventory* invenW = m_member[i].m_pUser->getCurCharacInvenW();
            invenW->checkEquipmentState(job, (int)growFirst, cntA, cntB, cntC,
                                        cntD, cntE, cntF);

            int lvl = m_member[i].m_pUser->get_charac_level();
            if (lvl > 0xf)
            {
                for (int k = 0; k < cntA; ++k)
                {
                    WongWork::CHackAnalyzer* hack =
                        m_member[i].m_pUser->getHackAnalyzer();
                    hack->addServerHackCnt(m_member[i].m_pUser,
                                           (WongWork::ENUM_HACKTYPE)0x1a4, 1, 0, 0);
                }
            }
            lvl = m_member[i].m_pUser->get_charac_level();
            if (lvl > 0x17)
            {
                for (int k = 0; k < cntB; ++k)
                {
                    int set = m_member[i].m_pUser->SetETC(0x1a5, cntC);
                    WongWork::CHackAnalyzer* hack =
                        m_member[i].m_pUser->getHackAnalyzer();
                    hack->addServerHackCnt(m_member[i].m_pUser,
                                           (WongWork::ENUM_HACKTYPE)0x1a5, 1,
                                           (unsigned int)set, 0);
                }
            }
            for (int k = 0; k < cntD; ++k)
            {
                WongWork::CHackAnalyzer* hack = m_member[i].m_pUser->getHackAnalyzer();
                hack->addServerHackCnt(m_member[i].m_pUser,
                                       (WongWork::ENUM_HACKTYPE)0x1a7, 1, 0, 0);
            }
            for (int k = 0; k < cntE; ++k)
            {
                WongWork::CHackAnalyzer* hack = m_member[i].m_pUser->getHackAnalyzer();
                hack->addServerHackCnt(m_member[i].m_pUser,
                                       (WongWork::ENUM_HACKTYPE)0x1a8, 1, 0, 0);
            }
            for (int k = 0; k < cntF; ++k)
            {
                WongWork::CHackAnalyzer* hack = m_member[i].m_pUser->getHackAnalyzer();
                hack->addServerHackCnt(m_member[i].m_pUser,
                                       (WongWork::ENUM_HACKTYPE)0x1a9, 1, 0, 0);
            }
        }
    }

    // 封印门判定：地狱图+类型1+旗标已立时必须消耗成功，否则整队失败。
    if (m_hellWorldMap != 0 && m_hellWorldMap->IsInHellDungeon() &&
        dtype == 1 &&
        m_sealDoorFlag != 0 &&
        UseSealDoorItems(m_hellWorldMap->m_field34) == 0)
        return 1;

    if ((char)dungeon->m_bloodDungeonType < 1)
    {
        if (!dungeon->isTournamentDungeon())
        {
            packet.clear();
            packet.put_header(0, 0x1c);
            packet.put_short(dungeonIdx);
            packet.put_byte((int)(char)type);
            bf.MakePacket(0x1c, &packet);
            send_to_party(packet);
            packet.clear();
            bf.MakePacket(0x1d, &packet);
            send_to_party(packet);
            _Send_TOD_APCInfo(todApcIdx);
        }
        else
        {
            packet.clear();
            packet.put_header(0, 0x182);
            packet.put_short(dungeonIdx);
            packet.put_byte((int)(char)type);
            bf.MakePacket(0x182, &packet);
            send_to_party(packet);
            packet.clear();
            bf.MakePacket(0x183, &packet);
            send_to_party(packet);
        }
    }
    else
    {
        packet.clear();
        packet.put_header(0, 0xd4);
        packet.put_short(dungeonIdx);
        packet.put_byte((int)(char)type);
        bf.MakePacket(0xd4, &packet);
        send_to_party(packet);
        packet.clear();
        bf.MakePacket(0xd3, &packet);
        send_to_party(packet);
        if (!bf.setBloodState((ENUM_BLOOD_STATE)2))
            return 1;
    }

    // 计时/奖励旗标区复位（BattleData +0x10..+0x38）
    bd.m_field20 = (int)GlobalData::s_systemTime_.getCurTickCount();   // +0x34c
    bd.m_field24 = 0;                                                  // +0x350
    bd.m_field10 = 0;                                                  // +0x33c
    bd.m_field14 = 0;                                                  // +0x340
    bd.m_field18 = 0;                                                  // +0x344
    bd.m_field1c = 0;                                                  // +0x348
    bd.m_field2c = 0;                                                  // +0x358
    bd.m_field30 = 0;                                                  // +0x35c
    bd.m_field34 = bd.m_field20;                                       // +0x360
    bd.m_field38 = bd.m_field24;                                       // +0x364

    set_state(2);
    SetEPLPState(1);
    gen_timer_key((TIMER_MESSAGE)0x2d);
    SetSelectedEPLPCmd(-1);
    int startCount = get_member_count();                   // -0x5c
    setStartGamePartyCount(startCount);

    secretshop::SECRET_SHOP_DATA& shop = m_secretShop;
    if (((char)dungeon->m_bloodDungeonType < 1) && m_manager->get_state() != 10 &&
        dungeon->isTowerOfDespairDungeon() == 0 &&
        dungeon->isTournamentDungeon() == 0)
    {
        // b28d-49 @0x85a18c6：ORIG 第三实参是重调 get_member_count()（`mov %esi,0xc(%esp)`），
        // 非 startCount 缓存（P157/P0；P167 MISSING get_member_count -1）。
        shop.m_openCode = GlobalData::s_secret_shop->LotteryNpc(
            dungeon->get_index(), dungeon->get_standard_level(), get_member_count());
    }
    else
    {
        shop.m_openCode = 1000;
    }

    char premiumCard = 0;                                  // -0x55
    unsigned int cardItem = 0;                             // -0x8c
    CEventBase* ev41 = GlobalData::s_event_manager->GetRepeatEvent(0x41);
    if (ev41->IsEventing(0) && IsReturnUserParty())
    {
        premiumCard = isPremiumGoldCardReward(m_battleField.m_pDungeon, cardItem) ? 1 : 0;
        if (premiumCard != 0)
        {
            SetPremiumGoldCardParty();
            SetPremiumGoldCardDefaultItem(cardItem);
        }
    }

    m_recvFlag.Clear();   // +0x1dc

    for (int i = 0; i <= 3; ++i)
    {
        for (int j = 0; j <= 1; ++j)
            m_field204[j][i] = (char)0xff;                 // +0x204[j][i]
        m_dungeonClearState = 0;                           // +0x1e0
        if (!_checkValidUser(i))
            continue;

        GlobalData::s_secret_shop->SendSecretShopNpc(m_member[i].m_pUser, shop.m_openCode);
        m_member[i].m_pUser->getCurCharacQuestW()->set_authen_data((QUEST_CONDITION)0,
                                                  dungeonIdx, (int)(char)type);
        if (dungeon != 0)
            SetProperLevelDungeon(m_member[i].m_pUser);

        if (partyType != 1)
        {
            bool fatigueSkip = (dungeon->m_villageAttackEventFlag == 1) || (dungeon->m_bloodDungeonType != 0) ||
                               (dungeonIdx == 10000) ||
                               (dungeon->isNoFatigueDungeon() == 1);
            if (!fatigueSkip)
            {
                if (!dungeon->isTournamentDungeon())
                {
                    m_member[i].m_pUser->FatigueUp(0);
                }
                else
                {
                    m_member[i].m_pUser->FatigueUp(dungeon->getTournamentRoundFatigue());
                    m_member[i].m_pUser->SendFatigue();
                }
            }
        }

        m_member[i].m_pUser->setPlayingDungeonIndex(dungeonIdx);
        m_member[i].m_pUser->m_character.PlayInfo::Reset();    // ORIG PlayInfo::Reset @ user+0x8e034（不清 m_f3c）
        m_member[i].m_pUser->m_character.m_f20 = startCount;   // user+0x8e054
        m_member[i].m_pUser->m_hasCharacterStatistic = 0;                 // user+0x8e074（CUser 直属字段；Layout_D 布局修正）

        // b28d-49 @0x85a1c34：ORIG 无 stdLvl/muLvl 缓存 —— 条件里逐点重调
        // get_standard_level（85a1c34/85a1c6b）+ get_charac_level（85a1c56/85a1c8d），
        // 每项 `lea ±5/±3` 就地折叠（P157/P0；P167 各 MISSING -1）。
        bool properLevel =
            ((m_member[i].m_pUser->get_charac_level() >=
              m_battleField.m_pDungeon->get_standard_level() - 5) &&
             (m_member[i].m_pUser->get_charac_level() <=
              m_battleField.m_pDungeon->get_standard_level() + 3));
        if (properLevel)
        {
            bd.m_flag28[0] = 1;                // +0x354
            bd.m_flag28[1] = 1;                // +0x355
        }

        int maxBefore = getMemberMaxLevelBeforeDungeon();
        int lvBefore = m_member[i].m_pUser->get_charac_level_before_dungeon();
        if ((maxBefore - lvBefore >= 7) && (startCount != 1))
        {
            bd.m_flag28[2] = 1;                // +0x356
            if (isGuildParty() ||
                FindUserByCharId(m_member[i].m_pUser->get_connect_upper_member()))
                bd.m_flag28[3] = 0;            // +0x357
            else
                bd.m_flag28[3] = 1;
        }
        else
        {
            bd.m_flag28[2] = 0;
            bd.m_flag28[3] = 0;
        }

        // [R5b-Main · #27] ORIG 0x085a1d9a `call get_dimension_possible; test %al,%al;
        //   setg %al`（0x085a1da1）= **有符号** >0（同 clear_reward.inc:64 族）。
        if ((signed char)m_battleField.m_pDungeon->get_dimension_possible() > 0)
        {
            bd.m_flag28[0] = 1;
            bd.m_flag28[1] = 1;
        }

        HistoryLog::WriteDunStart((_IO_FILE*)m_member[i].m_pUser->m_historyLogFp,
                                  m_member[i].m_pUser->get_charac_job(),
                                  m_member[i].m_pUser->get_charac_level(),
                                  (int)m_member[i].m_pUser->getCurCharacGrowType());

        if (getManager()->getMoveSpace() != 1)
        {
            // b28d-49 @0x85a1e6e：ORIG 按 `cmpl $0x1,-0x6c(%ebp); je` 分两臂各发一次
            // IncrementDungeonEntrance（85a1e8b false / 85a1ea9 true），非 bool 实参直传
            //（P167 MISSING GetInstanceDungeonEntranceLog/IncrementDungeonEntrance 各 -1）。
            if (partyType != 1)
                GetInstanceDungeonEntranceLog()->IncrementDungeonEntrance(
                    dungeonIdx, false);
            else
                GetInstanceDungeonEntranceLog()->IncrementDungeonEntrance(
                    dungeonIdx, true);
        }
        if (partyType != 1)
            m_member[i].m_pUser->IncCurCharacDungeonPlayCount();
        if (partyType == 1)
        {
            user_creature::CCreatureMgr* cm =
                const_cast<CInventory*>(m_member[i].m_pUser->getCurCharacInvenR())->GetCreatureMgrW();
            cm->TurnStomach(false);
        }
        m_member[i].m_pUser->set_level_before_dungeon((short)m_member[i].m_pUser->get_charac_level());

        if (dungeon != 0)
            SetProperDungeonEvent(m_member[i].m_pUser, dungeonIdx);
        SetEndRouting();
        static_cast<XNuclear::CHades*>(m_member[i].m_pUser->getHades())->StdCateUpdate(2, 0);
    }

    for (int a = 0; a <= 1; ++a)
        for (int b = 0; b <= 3; ++b)
            m_field1e4[a][b] = 0;                          // +0x1e4[a][b]

    Reset_party_overlapped_drop_ratio();
    check_passive_skill();
    send_loadingcheck_msg();

    CEventBase* ev57 = GlobalData::s_event_manager->GetRepeatEvent(0x57);
    // b28d-49 @0x85a2076：ORIG 两臂各发一次 SetWeekendEvent（85a2086 true / 85a209b false），
    // 三目形只发一次（P167 MISSING SetWeekendEvent -1）。
    if (ev57->IsEventing(0))
        SetWeekendEvent(true);
    else
        SetWeekendEvent(false);

    QuickParty::RewardData_Param rewardParam(this);
    QuickParty::RewardData_Result rewardResult;
    set_random_reward((QuickParty::RandomBuffType)9, rewardParam, rewardResult);
    SendPartyMemberTagChracInfo();
    return 0;
}