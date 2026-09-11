// ============================================================================
// CParty::kill_monster —— ORIG T 0x085a27e8（kill_monster.md，0x1381 字节）。
// 关键字段映射（汇编逐指令核对）：
//   STKillMonsterData：uid=monsterIdx, mapIdx, dieReason=memberCount,
//     +8=avgMoneyPercent, +0xc=minStdDropRate, +0x10/+0x11=die[0xa22/0xa23],
//     +0x14=set_random_reward(DROPPROB)->result.m_rate14
//   奖励分表：G_CDataManager()->m_serverParameter.m_expGroup368[memberCount-1]
//   击杀计数：BattleData(+0x32c).m_killedMonster0/1/2、m_field48、m_field4c
//   金卡掉落上限：G_CDataManager()->m_serverParameter.m_4968（dm+0x49d0），
//     当前地图 m_f0（CMap+0xf0）非 0 则覆盖；命中上限时 m_field48 不增。
//   维度判定：dungeon+0x89e / +0x89f（Cheat 维度标志）与 party short +0xc4c。
// [M5A_DispLink] 语义修复（ORIG 0x085a35e0 `mov -0x2c(%ebp),%eax; mov %eax,-0x468(%ebp)`）：
//   set_random_reward 前源码有 `expResult.m_value00 = exp` —— RewardData_Result.m_value00
//   是 **in-out 输入**（KillMonsterExp::process T 0x0826d198 读初值
//   `result.m_value00 = (int)((float)result.m_value00 * rate)`），缺此句则 buff 命中时
//   exp 被乘成 0（真语义 bug）。补齐后 OURS 1279+2 = 1281 = ORIG。
// ============================================================================
bool CParty::kill_monster(CUser* user, int monsterIdx, unsigned short mapIdx,
                         MSG_MONSTER_DIE const& dieInfo, int* param)
{
    // ORIG 0x085a27fe/0x085a280c：map_monster 与 PacketGuard 于函数入口构造
    // （先于 get_member_count，且 mob 先于 packet）——声明序即构造序（P29/P0）。
    map_monster mob;
    PacketGuard packet;

    int memberCount = get_member_count();

    // ORIG：地图怪物分区进入时记录当前击杀者席位到 BF。
    if (m_battleField.m_pDungeon != 0 && m_battleField.m_pDungeon->IsEnterEachMap())
        m_battleField.SetCurSeatNo(get_party_seatno(user));

    int avgMoneyPercent = 0;
    int sumMoneyPercent = 0;
    int validCount = 0;
    int minStdDropRate = 100;

    for (int i = 0; i < 4; ++i)
    {
        if (!_checkValidUser(i))
            continue;

        int moneyPercent = 0;
        int limit = G_CDataManager()->GetMoneyLimitPerLevel(
            m_member[i].m_pUser->get_charac_level(),
            m_member[i].m_pUser->get_acc_name());
        if (limit < 1)
        {
            cMyTrace trace(__PRETTY_FUNCTION__, 0x1414, 5);
            trace("WarField::HandleMonsterKill limitMoney(%d) is under 0", limit);
        }
        else
        {
            moneyPercent =
                (m_member[i].m_pUser->getCurCharacMoney() * 100) / limit;
            sumMoneyPercent += moneyPercent;
            ++validCount;
        }

        if (ARAD::Singleton<ServiceRestrictManager>::Get()->isRestricted(
                RestrictCategory::Enum_3, 1))
            minStdDropRate = std::min(
                minStdDropRate, m_member[i].m_pUser->getStdDropRate());
    }
    if (validCount != 0)
        avgMoneyPercent = sumMoneyPercent / validCount;

    // ---- STKillMonsterData 载荷 + 随机掉落概率 buff ----
    STKillMonsterData data;
    data.m_monsterUid = (unsigned short)monsterIdx;
    data.m_mapIndex = mapIdx;
    data.m_dieReason = (char)memberCount;
    data.m_field08 = avgMoneyPercent;
    data.m_field0c = minStdDropRate;
    data.m_flag10 = (char)(dieInfo.m_fieldA22 != 0);
    data.m_flag11 = (char)(dieInfo.m_fieldA23 != 0);

    QuickParty::RewardData_Param randParam(this);
    QuickParty::RewardData_Result randResult;
    set_random_reward(QuickParty::RANDOM_BUFF_INCREASE_DROPPROB, randParam, randResult);
    data.m_field14 = randResult.m_rate14;

    // ORIG 0x085a2d07 无 reward 清零（-0x3f4 仅被 lea/mov 读取）⇒ 不写 =0。
    unsigned int reward;
    if (!m_battleField.kill_monster(packet, mob, reward, data))
    {
        // ORIG 失败分支：非 Boss 房且（自动创建或单人）时上报 hack 0xcf。
        if (!checkBossRoom())
        {
            // ORIG 0x085a2b6d-0x085a2b91：短路 guard（无具名 bool 槽）。
            if (IsAutoCreated() != 0 || get_member_count() == 1)
                user->getHackAnalyzer()->addServerHackCnt(
                    user, (WongWork::ENUM_HACKTYPE)0xcf, 1, 0, 0);
        }
        return false;
    }

    if (param != 0)
        *param = (int)reward;

    // ---- 击杀合法化 hack 检测（0x25e：地图存活计数异常；0x25f：HP 上限异常） ----
    if (mob.m_field20 == 0 && !m_battleField.IsClearDungeon())
        user->getHackAnalyzer()->addServerHackCnt(
            user, (WongWork::ENUM_HACKTYPE)0x25e, 1, 0, 0);
    if (mob.m_field24 < mob.m_active.getHpMax() && !m_battleField.IsClearDungeon())
        user->getHackAnalyzer()->addServerHackCnt(
            user, (WongWork::ENUM_HACKTYPE)0x25f, 1, 0, 0);

    // ---- 成员击杀回调 + 击杀角色统计 ----
    OnKilledMonster(user, (int)reward, mob, dieInfo);

    char roleType = m_battleField.convertAPCRoleType2MonsterRoleType(
        mob.m_roleType);
    if (roleType == 1)
    {
        ++LINK2_BD(this).m_killedMonster1;
    }
    else if (roleType == 3)
    {
        ++LINK2_BD(this).m_killedMonster2;
    }
    else
    {
        ++LINK2_BD(this).m_killedMonster0;
        ++LINK2_BD(this).m_field4c;
        int expLimit = G_CDataManager()->m_serverParameter.m_4968;
        // P183：ORIG 逐处重取 GetCurrentMap（无 curMap 具名槽），删除缓存。
        if (m_battleField.GetCurrentMap() != 0)
        {
            if (m_battleField.GetCurrentMap()->m_f0 != 0)
            {
                expLimit = m_battleField.GetCurrentMap()->m_f0;
            }
        }
        // ORIG 0x085a39a6 `jne` 到第二条件 + 两份独立 ++m_field48（0x085a39a8/0x085a39b7）
        // ⇒ 源码是 if / else-if 链而非 || 合并（P133）。
        if (expLimit == 0)
            ++LINK2_BD(this).m_field48;
        else if (LINK2_BD(this).m_field4c <= expLimit)
            ++LINK2_BD(this).m_field48;
    }

    // ---- 击杀追踪器（m_manager 存在且全网格清时 traceEnd + 全员人证） ----
    if (m_manager != 0)
    {
        LINK2_TRACEHACK(this).setMobKiller((int)mapIdx);
        if (m_battleField.check_grid_clear())
        {
            CUser* users[4];
            users[0] = 0; users[1] = 0; users[2] = 0; users[3] = 0;
            users[0] = m_manager;
            LINK2_TRACEHACK(this).traceEnd(m_manager, users);
            for (int i = 0; i < 4; ++i)
            {
                if (_checkValidUser(i))
                    m_member[i].m_pUser->reqHumanCertify4ClearMap(true);
            }
        }
    }

    // ---- 副本清图追踪 ----
    if (roleType == 3)
        m_battleField.m_dungeonClearTracer.IncrementKilledBossCount();
    m_battleField.m_dungeonClearTracer.IncrementKilledMonsterCount();

    // ---- 任务怪物登记（存活成员；live 标志 1；非 PvP 房） ----
    for (int i = 0; i < 4; ++i)
    {
        if (!_checkValidUser(i))
            continue;
        if (m_battleData.m_arr54[i] != 1 || m_battleField.m_bloodState == 1)
            continue;
        int questEnemyType = 1;
        if (mob.m_roleType > 4 && mob.m_roleType <= 8)
            questEnemyType = 2;
        m_member[i].m_pUser->CheckQuestMonster(
            m_battleField.get_dungeon_index(), (int)reward,
            (ENUM_QUEST_ENEMY_TYPE)questEnemyType);
    }

    // ---- 击杀者金币历史（m_field796f8 为 _IO_FILE* 位模式） ----
    for (int i = 0; i < 4; ++i)
    {
        // ORIG 0x085a31d9/0x085a31e5 两条件各自 jne 跳统一失败出口（P133）：
        // 拆独立 if 以避免 4.4 的 bool 物化（mov $1/$0/test）。
        if (!_checkValidUser(i))
            continue;
        if (m_member[i].m_pUser == user)
            HistoryLog::WriteKillMob(
                reinterpret_cast<FILE*>(user->m_field796f8), (int)reward);
    }

    // ---- 总经验计算（成员数倍率 + 队伍经验加成事件） ----
    // ORIG 0x085a30dd-0x085a30eb：`jle` 到 1.0f 块、两臂共用同一 store
    // ⇒ 三目形（P47），非 if/else 赋值。
    float expRate = (memberCount < 1)
        ? 1.0f
        : G_CDataManager()->m_serverParameter.m_expGroup368[memberCount - 1];

    expRate +=
        static_cast<CPartyExpBonusEvent*>(
            GlobalData::s_event_manager->GetRepeatEvent(0x18))
            ->getBonusRate(memberCount, isHelpAbuseParty());

    unsigned int totalExp = getMonsterTotalExp(
        reward, roleType, m_battleField.get_dungeon_diff(),
        m_battleField.m_pDungeon->get_exp_weight(), expRate,
        mob.isNamedMonster());

    // ---- 击杀历史日志（KillMonster） ----
    user->m_historyLog.KillMonster(
        (int)reward,
        dieInfo.m_killValue1, dieInfo.m_killValue2,
        dieInfo.m_killPairA24, dieInfo.m_killPairA26, dieInfo,
        mob.m_roleType, mob.m_roleType == 5,
        user->getCurCharacExp(), (int)totalExp,
        // ORIG 0x085a3ab1 为具名局部（-0x68），但 4.4 不复用该槽，声明局部净增
        // 4 条指令（N82）；值恒 1，行为等价，故保留常量形。
        (eExpAddReason)1);

    const int memberLevelGap = _getMemberLevelGap();   // ORIG -0x64（写后未读）
    (void)memberLevelGap;

    // ---- PvP 胜点（角色类型 4+，怪物掉率 100/200） ----
    int winPoint = 0;
    if (mob.m_roleType > 4)
    {
        // P140：ORIG 直接解引用 dm+0x869c（无判空），照抄。
        CAICharacter* aiChar = G_CDataManager()->m_aiCharacterList->get(reward);
        if (aiChar != 0 &&
            (mob.m_dropCnt == 100 || mob.m_dropCnt == 200))
        {
            winPoint = aiChar->getPvPWinPoint();
        }
    }

    // ---- 逐成员经验分配（die[0xa22]==0 时执行） ----
    for (int i = 0; i < 4 && dieInfo.m_fieldA22 == 0; ++i)
    {
        // ORIG 0x085a355f/0x085a3565 两守卫各自跳统一出口（P133）：拆独立 continue
        // 以避免 4.4 的 bool 物化（mov $1/$0/test）。
        if (!_checkValidUser(i))
            continue;
        if (m_battleField.m_bloodState == 1)
            continue;

        // 未进入地图且非维度绕行 → 0 经验包
        // P140：ORIG 直接解引用 this+0xcac（无判空），照抄。
        if (m_battleData.m_arr58[i] != 1 &&
            m_battleField.m_pDungeon->m_bloodDungeonType == 0)   // +0x89f
        {
            m_member[i].m_pUser->SendZeroExpPacket();
            continue;
        }

        // ORIG 0x085a3369-0x085a3370：characLevel 先赋值、exp 后清零（声明序即初始化序）。
        int characLevel = m_member[i].m_pUser->get_charac_level();
        unsigned int exp = 0;
        // 维度未启用进入限制的完整单人份额计算。
        // ORIG 0x085a33b2 `jne A` / 0x085a33c1 `je B`：普通份额块（A）落在前、
        // get_index()==100 / 惩罚块（B）落在后 ⇒ 按 P59 写取反 guard 形。
        if (m_battleField.m_pDungeon->m_89e != 0 ||                     // +0x89e
            *(unsigned short*)((char*)this + 0xc4c) != 0)
        {
            // ORIG 复用 reward 槽（-0x3f4）作 get_mob_reward 出参且无清零。
            unsigned int mobReward;
            G_CDataManager()->get_mob_reward(characLevel, mobReward);
            unsigned int total = getMonsterTotalExp(
                mobReward, roleType, m_battleField.get_dungeon_diff(),
                m_battleField.m_pDungeon->get_exp_weight(), expRate, false);
            exp = total / (unsigned int)memberCount;
        }
        else
        {
            if (m_battleField.m_pDungeon->get_index() == 100)
            {
                // ORIG 复用 reward 槽（-0x3f4）作 get_mob_reward 出参且无清零。
                unsigned int mobReward;
                G_CDataManager()->get_mob_reward(characLevel, mobReward);
                unsigned int total = getMonsterTotalExp(
                    mobReward, roleType,
                    *reinterpret_cast<int*>(&this->m_battleField.m_dungeonDiff),   // +0xcc4（char 首字节起 4 字节块）
                    m_battleField.m_pDungeon->get_exp_weight(), expRate, false);
                exp = total / (unsigned int)memberCount;
            }
            else
            {
                float penalty = G_CDataManager()->BaseExpPenalty(
                    characLevel, (int)(unsigned char)mob.m_level);
                int memberPenalty = MemberPenaltyRate();
                // ORIG 0x085a3391-0x085a33a9：无 +0.5f（fistpl 前仅置截断控制字 0xc），
                // 且 x87 乘序为 (memberPenalty * penalty) * totalExp（浮点结合序不可换）。
                exp = (unsigned int)(
                    (float)memberPenalty * penalty * (float)totalExp /
                    (float)memberCount);
            }
        }

        // ---- 随机 buff（经验类） ----
        QuickParty::RewardData_Param expParam(this);
        QuickParty::RewardData_Result expResult;
        // ORIG 0x085a35e0：result.m_value00 为 in-out（process 读初值乘 rate）。
        expResult.m_value00 = (int)exp;
        set_random_reward(QuickParty::RANDOM_BUFF_INCREASE_EXP, expParam, expResult);
        exp = (unsigned int)expResult.m_value00;

        if (winPoint != 0)
            m_member[i].m_pUser->gainWinPoint(winPoint, (eWPAddReason)2);

        STGainedEachExpData expData;
        expData.m_exp = (int)exp;
        expData.m_expPartyDivisor = expRate;
        exp = sumGainedEachExp(m_member[i].m_pUser, expData);

        // 帮助滥用比例（维度均未启用）
        if (m_battleField.m_pDungeon->m_89e != 1 &&                    // +0x89e
            *(unsigned short*)((char*)this + 0xc4c) == 0)
        {
            m_member[i].m_pUser->calcHelpAbuseRatio(
                exp, isHelpAbuseParty(m_member[i].m_pUser));
        }

        // ORIG 0x085a3811 `movl $0x0,-0x474`：gainedSp 在滥用比例之后即清零
        // （早于 m_89f 绕行检查），与 gainedSfp 的 0x085a3855 分开两处。
        int gainedSp = 0;

        // 维度绕行：跳过蓝宝石倍率与经验发放
        if (m_battleField.m_pDungeon->m_bloodDungeonType != 0)
            continue;                                    // +0x89f

        // ---- 蓝宝石/迷宫倍率 ----
        if (m_member[i].m_pUser->getMoveSpace() == (UserSpace::T)1)
        {
            BlueMarble* marble =
                (BlueMarble*)m_member[i].m_pUser->getBlueMarble();
            if (marble != 0)
            {
                int buff = marble->getBuff(
                    m_member[i].m_pUser, (BlueMarbleBuffType::T)0);
                if (buff > 1)
                    exp = (unsigned int)(buff * (int)exp);
            }
        }

        // ORIG 0x085a3855 `movl $0x0,-0x478`：gainedSfp 在 gain_exp_sp 前清零。
        int gainedSfp = 0;
        if (m_member[i].m_pUser->gain_exp_sp(
                (int)exp, gainedSp, gainedSfp,
                (CUser::eExpAddReason)1, (int)reward, false))
        {
            m_member[i].m_pUser->SendNotiPacket((CUser::eSendTarget)2,
                                                (ENUM_NOTIPACKET)2, 1);
        }
    }

    // ---- 怪物疲劳统计（非命名怪且非 Boss/特殊） ----
    // m_mapHitList2[m_mapHitCount]（+0x64c*4+4）累加经验类值；
    // m_mapHitList3[m_mapHitCount]（+0x678*4+0xc）累加耗时类值。
    if (!mob.isNamedMonster() && roleType != 1 && roleType != 3)
    {
        ++m_field1ab8;
        // ORIG 0x085a3a17-0x085a3a29 下标 m_mapHitCount 被读取两次且用 lea 合成
        // ⇒ 源码为显式 a[i] = a[i] + v（非 +=，P38 同族）。
        m_mapHitList3[m_mapHitCount] =
            m_mapHitList3[m_mapHitCount] + (int)dieInfo.m_killValue1;
        m_mapHitList2[m_mapHitCount] =
            m_mapHitList2[m_mapHitCount] + (int)dieInfo.m_killValue2;
    }
    if (dieInfo.m_killValue1 > 9 && dieInfo.m_killValue1 % 10 == 0)
        m_field1aa0 = 1;

    // ---- 安全检测上报 + 广播 ----
    // +0x1abc = m_secu（Secu_HackLogCheckByParty 具名成员，CParty.h 字段化后）
    m_secu.DieMob(&dieInfo, &mob);

    send_to_party(packet);

    // ---- Boss 击杀广播（header 0x32，单字节房间号） ----
    if (roleType == 1)
    {
        PacketGuard bossPacket;
        // ORIG 0x085a3b0e..0x085a3b45 每次调用均 `lea SLOT,%eax; mov %eax,(%esp)`
        // （无具名 buf 指针槽、无匿名临时槽）⇒ 引用 cast 逐点写表达式。
        reinterpret_cast<InterfacePacketBuf&>(bossPacket).clear();
        reinterpret_cast<InterfacePacketBuf&>(bossPacket).put_header(0, 0x32);
        // +0xccc = m_battleField(+0xb24) + 0x1a8 = m_apcKillCount（CBattle_Field.h:459）
        reinterpret_cast<InterfacePacketBuf&>(bossPacket).put_byte(m_battleField.m_apcKillCount);
        reinterpret_cast<InterfacePacketBuf&>(bossPacket).finalize(true);
        send_to_party(bossPacket);
    }

    prepare_dungeon_clear();

    // ---- 清图条件判定（roleType→条件类型） ----
    int clearConditionType;
    if (mob.m_roleType == 3)
        clearConditionType = 4;
    else if (mob.m_roleType < 3 || (unsigned int)(mob.m_roleType - 5) > 3)
        clearConditionType = 2;
    else
        clearConditionType = 3;

    if (m_battleField.ClearCondition(
            (ENUM_DUNGEON_CLEAR_CONDITION_TYPE)clearConditionType,
            (int)reward))
    {
        ClearDungeon();
    }
    return true;
}