// ============================================================================
// CParty::beginBloodClearProcess —— ORIG 0x85b75d4：血轮结算主流程
// （状态机推进 → 逐成员奖励/经验/记录包 0xd0 → 0x23 定时器）。
// ============================================================================
// b28d-solo-CParty: N 封顶 —— N82 帧 0xb0/0xc0；404 vs 475
// [G7-sub 2026-09-10 复核·写域边界] 函数体（ORIG 0x085b75d4..0x085b7c9a，478 条）在
// CParty.cpp:2013 起，本 .inc 无源码行 ⇒ 本轮无改动。缺口/帧差请由 CParty.cpp 属主处理。
// 已知：P167 CLEAR（仅 PacketGuard/RewardData C1-C2·D1-D2 别名）；bf 视图宏重物化已前轮落地。
void CParty::beginBloodClearProcess(bool flag)
{
    // b28d-49 判词：N 封顶（N82 帧/槽）。P167 CLEAR。@0x85b75d4
// §P331 第二形（实测 709→666）：ORIG 每次 bf 成员访问都 `mov 0x8(%ebp),%eax;
// add $0xb24,%eax` 重物化（add$0xb24 8 处，本方原 1 —— 引用起基址吸收了 7 处）。
// 改宏每次直取：bf 全程只读该视图、this 不变 ⇒ 寻址同物，无可观察行为变化。
#define bf (*reinterpret_cast<CBattle_Field*>(&m_battleField))
    if (bf.getBloodState() >= 3)
        return;

    int playTime = 0;                                   // -0x34
    int round = 0;                                      // -0x40[0]
    int bestTime = 0;                                   // -0x30
    int bestRound = 0;                                  // -0x2c
    int ultimatePoint = 0;                              // -0x28
    playTime = bf.m_bloodReward.getPlayTime();
    if (flag)
    {
        round = bf.getBloodMaxRound();
        bf.m_bloodReward.setCurrentBloodRound(round);
    }
    else
    {
        round = bf.m_bloodReward.getCurrentBloodRound();
    }

    if (!bf.setBloodState((ENUM_BLOOD_STATE)3))
        return;

    // ORIG beginBloodClearProcess.md 085b76b1 `movzbl 0x89f(%eax)` =
    // CDungeon::m_bloodDungeonType（unsigned char 宽度修正）
    if (flag && m_battleField.m_pDungeon->m_bloodDungeonType == 2)
    {
        short maxRound = (short)bf.getBloodMaxRound();
        ultimatePoint =
            bf.getNextRoundDifficulty((short)(maxRound - 1));
        bf.m_bloodReward.incUltimateRoundClearPoint(ultimatePoint);
    }

    for (int slot = 0; slot < 4; ++slot)                // -0x20
    {
        if (!(get_user(slot) == 0 || !checkValidUser(slot)))
        {
            // P183 反向（ORIG 085b778b/77b2/77d9/7800 实证）：ORIG 无具名 user/ci
            // 缓存，每个使用点各自 9 条重算 m_member[slot].m_pUser（add $0x78 序列）。
            get_user(slot)->setChattingMessageCount(0);
            onBloodClear(flag, slot);
            reinterpret_cast<CUserCharacInfo*>(m_member[slot].m_pUser)
                ->set_charac_party_bonus_exp(0);
            reinterpret_cast<CUserCharacInfo*>(m_member[slot].m_pUser)
                ->set_charac_member_bonus_exp(0);
            reinterpret_cast<CUserCharacInfo*>(m_member[slot].m_pUser)
                ->set_charac_fatigue_buf_bonus_exp(0);
            reinterpret_cast<CUserCharacInfo*>(m_member[slot].m_pUser)
                ->set_charac_seria_buf_bonus_exp(0);

            checkBloodRenewMyRecord(slot, playTime, round,
                                    bf.get_dungeon_index());

            int itemMax = round * 10 / bf.getBloodMaxRound();   // -0x18
            if (itemMax == 0)
                itemMax = 1;
            int itemLimit = 10;                         // -0x40[1]
            int itemCount = std::min(itemLimit, round);
            // 报告 generateBloodRewardItem.md + ORIG 0x85b78ef/78e7 实证：第 2 参为
            // itemMax（setRewardItem/循环次数），第 4 参为 itemCount（field324 乘数）；
            // 原序把两者写反（round=0 时行为可见差异）。
            generateBloodRewardItem(slot, itemMax,
                                    (unsigned int)reinterpret_cast<CUserCharacInfo*>(
                                        m_member[slot].m_pUser)->get_charac_level(),
                                    (unsigned int)itemCount, flag);

            unsigned int mobReward = 0;                 // -0x44
            G_CDataManager()->get_mob_reward(
                reinterpret_cast<CUserCharacInfo*>(m_member[slot].m_pUser)
                    ->get_charac_level(),
                mobReward);
            QuickParty::RewardData_Param rp(this);
            QuickParty::RewardData_Result rr;
            rr.m_value00 = (int)mobReward;
            set_random_reward((QuickParty::RandomBuffType)0, rp, rr);
            mobReward = (unsigned int)rr.m_value00;

            // ORIG 085b799e/3d2 `add $0x6164,%eax; call 082f10ee`
            //（= _ZNKSt6vectorIfSaIfE4sizeEv，nm 实证）：0x6164 =
            // CDataManager::m_bloodDungeon(+0x5e30)+0x334 → m_vec334.size()。
            // 原 vptr 解引用+8 裸读形态错误，改真实 size() 调用（-O0 产 call）。
            int inoutSize =
                static_cast<int>(G_CDataManager()->m_bloodDungeon.m_vec334.size());
            // ORIG 085b79d0/404 `call GetCharacExpandData(1); call 085bfe88`
            //（= _ZNK18CCharacBloodDungeon13GetEnterCountEv，读 +0x18）——
            // 原本地视图读 +0x0c（m_weeklyEnterCount）为偏移错读，修正。
            int enterCount =
                reinterpret_cast<CCharacBloodDungeon*>(
                    m_member[slot].m_pUser->GetCharacExpandData(
                        (ENUM_CHARAC_EXPAND_TYPE)1))
                    ->GetEnterCount();   // -0x6c
            int expLimit = inoutSize - 1;               // -0x40[2]
            int inoutCnt = std::min(expLimit, enterCount);
            generateBloodRewardExp(slot, round, mobReward,
                                   (short)inoutCnt);

            unsigned int rewardExp =
                bf.m_bloodReward.getRewardExp(slot);    // -0x10
            if (1000000 < rewardExp)
            {
                LogManager::logFormat(
                    1, "party.cpp",
                    __PRETTY_FUNCTION__, 0x3c79,
                    "BLOOD_LOG : EROOR BLOOD CLEAR REWARD EXP(%u)\n",
                    rewardExp);
                rewardExp = 1000000;
                bf.m_bloodReward.setRewardExp(slot, 1000000);
            }
            if (round == 0)
            {
                playTime = 0;
                round = 0;
            }
            bestTime = (int)reinterpret_cast<CUserCharacInfo*>(
                m_member[slot].m_pUser)->getBloodBestTimeRecord();
            bestRound = reinterpret_cast<CUserCharacInfo*>(
                m_member[slot].m_pUser)->getBloodBestRoundRecord();
            if (bestRound == -1)
            {
                bestTime = playTime;
                bestRound = round;
            }
            PacketGuard packet;                         // -0x78
            packet.put_header(0, 0xd0);
            packet.put_int(playTime);
            packet.put_int(round);
            packet.put_int(bestTime);
            packet.put_int(bestRound);
            packet.put_int(bf.getBloodMaxRound());
            packet.put_int((int)bf.m_bloodReward.getRewardExp(slot));
            packet.finalize(true);
            m_member[slot].m_pUser->Send(packet);

            int key = gen_timer_key((TIMER_MESSAGE)0x23);
            G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)1,
                                        GetPartyIndex(), (TIMER_MESSAGE)0x23,
                                        8, key, 0x23);
        }
    }
}