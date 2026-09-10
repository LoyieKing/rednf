void CDeathTower::_beginTowerClearProcess(bool isLast)
{
    if (getTowerState() > 2)
        return;

    // [b28d-69] 声明序照 ORIG 槽（curStage -0x5c / playTime -0x40 预清 / memberCnt -0x3c）；
    // partyState/key/mgrIdx 均无槽往返 ⇒ 原码为表达式直用（P0），勿声明局部。
    unsigned int curStage;
    unsigned int playTime = 0;
    int memberCnt = m_playData.getStartMemberCnt();
    void* rankTable = WongWork::CDeathTowerRanking::getRankTable(memberCnt);

    _onClear(isLast);

    if (m_party->m_battleField.m_bloodState == 1)
    {
        setTowerState((ENUM_TOWER_STATE)5);
        m_party->SetEPLPState(2);

        G_TimerQueue()->InsertTimer(
            (TimerEntry::OBJ_TYPE)6, m_idx, (TIMER_MESSAGE)0x4d, 0x3c,
            _genTimerKey((TIMER_MESSAGE)0x4d), 0x4d);

        PacketGuard pkt;
        pkt.put_header(0, 0xfe);
        pkt.finalize(true);
        m_party->send_to_party(pkt);
        // [判据①] ORIG 0x08467375 `jmp 08467b14`（函数 epilogue）⇒ 原码 then 分支末尾 return；
        // 旧的 if/else 共享后续代码使 then 落点变成 0x084673dd（curStage 计算），语义偏离。
        return;
    }

    setTowerState((ENUM_TOWER_STATE)3);

    G_TimerQueue()->InsertTimer(
        (TimerEntry::OBJ_TYPE)6, m_idx, (TIMER_MESSAGE)0x4b, 1,
        _genTimerKey((TIMER_MESSAGE)0x4b), 0x4b);

    curStage = m_stage.getCurrentStage();
    if (isLast)
    {
        playTime = m_playData.getPlayTime();
    }
    else
    {
        playTime = m_playData.getLastPlayTime();
        int stageMinus1 = curStage - 1;
        int one = 1;
        curStage = std::max(one, stageMinus1);
    }

    for (int i = 0; i <= 3; ++i)
    {
        CUser* user = 0;
        user = m_party->get_user(i);
        if (!user || !m_party->checkValidUser(i))
            continue;

        user->set_charac_party_bonus_exp(0);
        user->set_charac_member_bonus_exp(0);
        user->set_charac_fatigue_buf_bonus_exp(0);
        user->set_charac_seria_buf_bonus_exp(0);

        // [b28d-69] P183 地址重物化铁律：ORIG 每个使用点重发 getDungeon()->get_index()/
        // getCurCharacNo()（p167 差集 getDungeon 11/get_index 9/getCurCharacNo 6 实证），
        // 禁缓存指针或索引；isNewRecord 的 P184 死块（内层再测 isLast）照抄。
        int isNewRecord = 0;
        int result = _checkRenewMyRecord(
            m_dungeonMgr.getDungeon()->get_index(), memberCnt,
            user->getCurCharacNo(), curStage, playTime);
        if (isLast)
        {
            // [b28d-SCT] P47/P73：ORIG 0x0846754f `cmpl $0x1,-0x28; jne L; mov $1;
            // jmp; L: mov $0` 分支物化；`isNewRecord = (result == 1)` 在 4.4 生
            // sete/movzbl（少 1 jne，jcc 47→46）。改写 if/else 赋值对齐 ORIG 形态。
            if (result == 1)
                isNewRecord = 1;
            else
                isNewRecord = 0;
            if (_checkRenewTopRecord(m_dungeonMgr.getDungeon()->get_index(), memberCnt,
                                     curStage, playTime) && isLast)
                isNewRecord += 2;
        }

        int virtualRank = ((WongWork::CDeathTowerRanking*)rankTable)->getVirtualRank(
            WongWork::CDeathTowerRanking::makeDungeonIdx2TowerIdx(
                m_dungeonMgr.getDungeon()->get_index()),
            curStage, playTime);

        // ORIG 0x0846760e..0x08467629：`cmp $0x2af8; sete; test; je` + 两臂 movb
        // ⇒ if/else 赋值形（非布尔表达式直存）。
        char notTower;
        if (m_dungeonMgr.getDungeon()->get_index() == 11000)
            notTower = 0;
        else
            notTower = 1;
        GetInstanceDeathTowerPlaydataStatistic()->SendDeathTowerPlayInfo(
            i, notTower,
            (const stDeathTowerRecordMemberInfo_t*)m_playData.getStartMemberInfo(),
            curStage, m_playData.getStartMemberCnt());

        GetInstanceDeathTowerValueStatistic()->SendDeathTowerValueStatistic(
            (DEATHTOWER_VALUE_STATISTIC_FIELD)1, notTower,
            (char)user->get_charac_level(), curStage);
        GetInstanceDeathTowerValueStatistic()->SendDeathTowerValueStatistic(
            (DEATHTOWER_VALUE_STATISTIC_FIELD)0, notTower,
            (char)user->get_charac_level(), 1);

        if (result != 0)
        {
            DB_UpdateDeathTowerBestRecord::makeRequest(
                WongWork::CDeathTowerRanking::makeDungeonIdx2TowerIdx(
                    m_dungeonMgr.getDungeon()->get_index()),
                user->getCurCharacNo(), memberCnt,
                (const stDeathTowerRecordMemberInfo_t*)m_playData.getStartMemberInfo(),
                curStage, playTime);
            if (virtualRank > 0)
            {
                GlobalData::s_monitor_proxy_mgr->GetServerProxy(
                    (ENUM_SERVER_GROUP)0)->updateTowerRank(
                        user->getCurCharacNo(),
                        WongWork::CDeathTowerRanking::makeDungeonIdx2TowerIdx(
                            m_dungeonMgr.getDungeon()->get_index()),
                        memberCnt, virtualRank);
                ((WongWork::CDeathTowerRanking*)rankTable)->updateRanking(
                    WongWork::CDeathTowerRanking::makeDungeonIdx2TowerIdx(
                        m_dungeonMgr.getDungeon()->get_index()),
                    user->getCurCharacNo(), virtualRank);
            }
            ((WongWork::CDeathTowerRanking*)rankTable)->registBestRecord(
                WongWork::CDeathTowerRanking::makeDungeonIdx2TowerIdx(
                    m_dungeonMgr.getDungeon()->get_index()),
                user->getCurCharacNo(),
                (const stDeathTowerRecordMemberInfo_t*)m_playData.getStartMemberInfo(),
                curStage, playTime);
        }

        CPacketHandler handler;
        handler.makeDisplayRangking(
            memberCnt, user->getCurCharacNo(), curStage, playTime, (result == 1),
            m_dungeonMgr.getDungeon()->get_index(), virtualRank);
        user->Send(handler);

        // 奖励数量：ORIG _beginTowerClearProcess 0x0846791f..0x08467948 两套
        // int 表（limitOfStackableItemInTower 决定）：`mov 0xc(dm+(tier+stage)*4)`
        // = m_deathTower.m_rewardCardNum[stage]（+0x324 基）/
        //   m_illusionRewardCardNum[stage]（+0x964 基）。
        int rewardNumBase;
        if (m_dungeonMgr.getDungeon()->limitOfStackableItemInTower())
            rewardNumBase = G_CDataManager()->m_deathTower.m_rewardCardNum[curStage];
        else
            rewardNumBase =
                G_CDataManager()->m_deathTower.m_illusionRewardCardNum[curStage];
        // ORIG 三次 min：min<unsigned>(maxCardNum,cardNum) → min<int>(10,rewardItemNum)
        // → min<unsigned>(10,curStage)（p167 差集 2×min<unsigned>+1×min<int> 实证）。
        unsigned int cardNum = (unsigned int)rewardNumBase + (unsigned int)isNewRecord;
        unsigned int maxCardNum =
            (unsigned int)m_dungeonMgr.getDungeon()->getTowerMaxClearRewardItemNum();
        int rewardItemNum = std::min(maxCardNum, cardNum);
        rewardItemNum = std::min(10, rewardItemNum);

        m_playData.generateRewardItem(i, rewardItemNum, m_dungeonMgr,
                                      user->get_charac_level(),
                                      std::min(10u, curStage), m_party);

        unsigned int mobReward = 0;
        G_CDataManager()->get_mob_reward(user->get_charac_level(), mobReward);
        QuickParty::RewardData_Param rewardParam(m_party);
        QuickParty::RewardData_Result rewardResult;
        rewardResult.m_value00 = mobReward;
        m_party->set_random_reward((QuickParty::RandomBuffType)0, rewardParam,
                                   rewardResult);
        mobReward = rewardResult.m_value00;

        m_playData.generateRewardExp(i, curStage, mobReward, m_party, m_dungeonMgr);
    }
