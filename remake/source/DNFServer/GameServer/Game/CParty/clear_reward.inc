// ============================================================================
// CParty::clear_reward —— ORIG T 0x085ae0d4（0x3119 字节）
//   报告：docs/class_func_reports/CParty/clear_reward.md（## 汇编 + ## 反编译 C
//   逐段对照转写）。浮点常量（df_game_r .rodata）：_DAT_08cbbbcc=100.0f、
//   _DAT_08cbbbec=10000.0f、0x08cbbbf0=double 100.0；fistp 就近舍入序列按工程
//   惯例以 (int)/(long long)(x+0.5) 转写。
//   stGenerateRefData_t 调用上下文区按 ORIG 原始偏移写入：
//     +0x34..+0x37 字节：stdLevel/diff/memberCount 打包（全程按字节写）；
//     +0x38 dword（m_rankBonusRate 视图）：rankBonusIdx → mapRatio → getGoldCardCreateRate
//     三次位型覆写（ORIG -0x210 槽 0x036a/0x0529/0x11db/0x1539 实证）；
//     +0x3c dword（m_field3c 视图）：金币半字打包与 premium 组号 0/3、1/4、5 复用；
//     +0x40 CDungeon*（第二遍整字写覆盖 +0x40..43 半字死写）。
//   奖励槽视图：BattleData::m_itemGroups{0,1,2}[slot][2]（this+0x3a0/+0x588/+0x770）。
// ============================================================================
// b28d-solo-CParty: N 封顶 —— N82 帧 0x3bc/0x460 + 大量缺口；2595 vs 3090
void CParty::clear_reward(int param)
{
    // b28d-49 判词：N 封顶（N82 帧/槽 + N89 STL 内联）。P167 CLEAR（327/327）。@0x85ae0d4
    // [轴 B 自校·实测] axisB_points.tsv 行（0x85b090a `cmp -0x30(%ebp),%eax; setg`，
    //   getClearRewardBonusExp 尾循环）按地址自校：我方同点为 `call vector::size;
    //   cmp …,%eax; setg`（setg×3）⇒ **两侧同为有符号**，表内序号错配假阳性，
    //   未改源码（另 ORIG `cmp $0x2; ja` 与我方 `setbe` 仅极性差异，符号性同为无符号）。
    CDataManager* dm = G_CDataManager();
    // [G7-sub] P0 槽律：ORIG 085ae0e1 `call get_dungeon_index` 结果**无槽**、直进
    // `0x4(%esp)` 作 find_dungeon 实参（085ae0e6）；我方原 `int dungIdx` 落
    // -0x1a4 槽（多 1 槽 + 2 条往返）⇒ 内联回实参位。
    CDungeon* dungeon = dm->find_dungeon(
        reinterpret_cast<CBattle_Field*>(&m_battleField)->get_dungeon_index());
    int memberCount = get_member_count();

    if (dungeon == 0)
    {
        LogManager::logFormat(1, "party.cpp", __PRETTY_FUNCTION__,
                              0x2a07, "Unknown Dungeon %d",
                              reinterpret_cast<CBattle_Field*>(&m_battleField)->get_dungeon_index());
        return;
    }
    if (memberCount < 1)
    {
        LogManager::logFormat(1, "party.cpp", __PRETTY_FUNCTION__,
                              0x2a0c, "No Member[%d] in party", memberCount);
        return;
    }
    if (dungeon->isTournamentDungeon())
    {
        LogManager::logFormat(1, "party.cpp", __PRETTY_FUNCTION__,
                              0x2a13, "TournamentDungeon(%d) can not call function",
                              reinterpret_cast<CBattle_Field*>(&m_battleField)->get_dungeon_index());
        return;
    }

// §P183：ORIG 无 bd/bfView 别名槽，每处访问都从 this 重物化
// （add $0x390×9、$0xb24×12、$0x79d×6、$0x10×16）；引用缓存会省掉这些
// 地址计算 ⇒ 宏化为「每次直取 this+off」，语义等价。
#define bd m_battleData
#define bfView (*reinterpret_cast<CBattle_Field*>(&m_battleField))

    // ORIG:local_ec(G_CDataManager 首调结果)复用于 find_dungeon/getRankBonusIndex/+0x68。
    int rankBonusIdx = dm->getRankBonusIndex(param);
    ServerParameterScript* spScript =
        &dm->m_serverParameter;                              // dm+0x68 子对象

    int stdLevel = 0;
    // [R5b-Main · #27] ORIG 0x085ae24a `call get_dimension_possible; test %al,%al;
    //   setg %al`（0x085ae251）= **有符号** >0；头声明 unsigned char ⇒ 无 cast 时 4.4 产
    //   `setne`，值 ≥0x80 分叉（R5a18 全仓扫描：ORIG 31 站点 setg=17/setne=0）。
    if ((signed char)dungeon->get_dimension_possible() > 0)
        stdLevel = getStandardDimensionLevel();
    else
        stdLevel = dungeon->get_standard_level();

    bd.m_field68c += 1;

    int gridCount = 0;
    // ORIG：getMaze() 在两个循环条件与体内各调用一次（不提取缓存）。
    for (int gy = 0; gy < bfView.getMaze()->m_width; ++gy)
    {
        for (int gx = 0; gx < bfView.getMaze()->m_height; ++gx)
        {
            const GridScript& grid = bfView.getMaze()->getGridR(gy, gx);
            if (grid.m_gridValue != 0)
                gridCount += 1;
        }
    }
    float mapRatio =
        static_cast<float>(bd.m_field68c) / static_cast<float>(gridCount);

    int ratePerMapMax =
        static_cast<WongWork::CClearReward*>(
            G_CDataManager()->getItemGenerator()->getGenerator(
                WongWork::CItemGeneratorMgr::eGenerateType_2))
            ->getRewardItemRatePerMapMaxCount(gridCount);
    if (ratePerMapMax != 0)
    {
        // [R5a07 ④] ORIG 0x085ae3cb..0x085ae3f6：`mov -0xc0(%ebp),%eax; mov $0x0,%edx;
        //   mov %eax,-0x310(%ebp); mov %edx,-0x30c(%ebp); fildll -0x310(%ebp)` ⇒ 高 32 位清零
        //   = **无符号 32 位**语义（(unsigned long long)(unsigned int)n）。
        //   原写 static_cast<unsigned long long> 使 4.4 发 `sar $0x1f,%edx`（符号扩展）+ 负值
        //   加 2^64 修正（fldt 2^64 + faddp，NEW 0x084b5355..0x084b5376）⇒ ratePerMapMax<0 时
        //   结果分叉（ORIG≈(unsigned)n=4.29e9 vs 我方≈1.8e19）。改无符号 32 位以复刻 ORIG。
        mapRatio = static_cast<float>(
                       static_cast<unsigned int>(ratePerMapMax)) *
                   mapRatio / 10000.0f;
    }

    WongWork::stGenerateRefData_t ref;
    ref.m_field35 = static_cast<char>(bfView.get_dungeon_diff());
    ref.m_field34 = static_cast<char>(stdLevel);
    ref.m_rateSlot = static_cast<char>(memberCount);
    ref.m_rankBonusIndex = rankBonusIdx;
    // ORIG 0x085ae447..0x085ae476：movw this+0x374→+0x3c、this+0x36c→+0x3e、
    // 0→+0x40、this+0x370→+0x42（半字视图，随后第二遍 m_battleField.m_pDungeon 整字写覆盖 +0x40..43）
    ref.m_goldBase = static_cast<unsigned short>(bd.m_field48);      // +0x3c
    ref.m_goldFactor = static_cast<unsigned short>(bd.m_killedMonster1);  // +0x3e
    ref.m_field40 = 0;                                               // +0x40
    ref.m_field42 = static_cast<unsigned short>(bd.m_killedMonster2);    // +0x42

    WongWork::stGenerateResult_t result;
    // ORIG:条件为 != 0(power_war 分支先发射,generateSpecificItem 为 else)。
    if (m_battleField.m_pDungeon->m_89e != 0)
    {
        // ORIG 0x085ae520 区：getGenerator 链在循环【前】求值一次（local_a8 缓存），
        // 循环体仅 checkValidUser→get_charac_level→power_war(...)。
        WongWork::CClearReward* powerGen =
            static_cast<WongWork::CClearReward*>(G_CDataManager()->getItemGenerator()->getGenerator(WongWork::CItemGeneratorMgr::eGenerateType_2));
        for (int i = 0; i < 4; ++i)
        {
            if (_checkValidUser(i))
            {
                powerGen->power_war_dungeon_gold_clear_reward(
                    ref, result, m_member[i].m_pUser->get_charac_level());
            }
        }
    }
    else
    {
        static_cast<WongWork::CClearReward*>(G_CDataManager()->getItemGenerator()->getGenerator(WongWork::CItemGeneratorMgr::eGenerateType_2))->generateSpecificItem(ref, result);
    }
    increase_result_card_gold(result);

    // ORIG 0x085ae55f/0x085ae57a：dropItems 迭代器 begin() 在第二遍 ref 赋值【前】
    // 构造一次（iterator→const_iterator 转换调用），跨 slot 递进消费；end() 每轮重取。
    std::vector<Inven_Item>::const_iterator dropIt = result.m_dropItems.begin();

    ref.m_field35 = static_cast<char>(bfView.get_dungeon_diff());
    ref.m_field34 = static_cast<char>(stdLevel);
    ref.m_rankBonusRate = mapRatio;                    // +0x38 float 视图（ORIG 0x085ae5fd）
    ref.m_dungeon = dungeon;

    int minStdDropRate = 100;
    for (int i = 0; i <= 3; ++i)
    {
        if (_checkValidUser(i))
        {
            // [R21-07 形态修复] ORIG 0x085ae63d/0x085ae649 无 receiver 具名槽
            // （`call ARAD::Singleton<ServiceRestrictManager>::Get; movl $1,0x8(%esp);
            // movl $3,0x4(%esp); mov %eax,(%esp); call isRestricted`）⇒ 原码为链式
            // 调用（P356/P183）；旧具名局部多发 `mov %eax,-0x150(%ebp)` +
            // `mov -0x150(%ebp),%eax` 两条 ⇒ 移除。
            if (ARAD::Singleton<ServiceRestrictManager>::Get()->isRestricted(
                    RestrictCategory::Enum_3, 1))
            {
                minStdDropRate =
                    std::min(m_member[i].m_pUser->getStdDropRate(), minStdDropRate);
            }
        }
    }

    char guildTodayFlag[4] = {0, 0, 0, 0};
    for (int i = 0; i <= 3; ++i)
    {
        if (_checkValidUser(i) && m_member[i].m_pUser->IsTodayGuildMember())
        {
            guildTodayFlag[i] = 1;
            for (int j = 0; j <= 3; ++j)
            {
                if (_checkValidUser(j) &&
                    m_member[i].m_pUser->get_charac_guildkey() ==
                        m_member[j].m_pUser->get_charac_guildkey())
                {
                    guildTodayFlag[j] = 1;
                }
            }
        }
    }

// §P183 形态核销：ORIG 对 m_member[slot].m_pUser 每次访问都从 this 重新物化
// （add $0x78 共 63 处；-0x58(%ebp) 槽 41 次），我方原以 `CUser* user` 缓存 ⇒
// 净缺 ~330 条地址计算指令。改宏为「每次直取」，指令数复现 ORIG，语义等价
// （slot 在两循环内均为循环变量，循环体无对 user 的赋值）。
#define user (m_member[slot].m_pUser)
    for (int slot = 0; slot <= 3; ++slot)
    {
        if (!_checkValidUser(slot))
        {
            continue;
        }

        if (m_battleField.m_pDungeon->m_entryRestrictFlag != 0 &&
            user->IsDeleteEventDungeonInvitation())
        {
            user->EventDungeonClearReward(m_battleField.m_pDungeon->m_vec86c);
        }
        if (G_CDataManager()->getNewRankIndex(param) < 3)
        {
            APSystem::CUserProc::ClearActionAndSendtoUser(
                user, (APSystem::_ActionGroupIndex)0x10, 0,
                static_cast<ENUM_PACKETCLASS>(0));
        }

#define rewardItems (bd.m_itemGroups0[slot])
#define goldCardItems (bd.m_itemGroups2[slot])
#define pcRoomItems (bd.m_itemGroups1[slot])
        if (m_battleField.m_pDungeon->m_89e == 0)
        {
            int lvl = user->get_charac_level();
            double basePenalty = G_CDataManager()->BaseExpPenalty(lvl, stdLevel);
            if (basePenalty >= 1.0 || m_battleField.m_pDungeon->isRiskDungeon())
            {
                unsigned int guildLvl = static_cast<unsigned int>(
                    static_cast<unsigned short>(
                        static_cast<const STGuildDBInfoOnly*>(
                            user->GetGuildDBInfo())->m_memberCount));  // +0x22
                unsigned int tempExp =
                    static_cast<unsigned int>(user->get_guild_temp_exp());
                // [R28a07 ④-精度轴] ORIG 0x085ae9b0/0x085ae9d3：`fildll tempExp; fstpl -0x360;
                //   call getExpWeight; fmull -0x360` —— getExpWeight 返回值**无** fstps 舍入
                //   ⇒ 原码局部类型是 **double**（getExpWeight 返回 double）。原写 float 多一次
                //   double→float 舍入（有效位 53→24），w 的小数位 >24bit 时 addExp 分叉。
                double weight = G_CDataManager()->m_guildParameter.getExpWeight(guildLvl);
                int addExp = static_cast<int>(
                    static_cast<double>(weight) *
                        static_cast<double>(tempExp));
                // ORIG:条件为 != 0(today 分支先发射,add_guild_exp 为 else)。
                if (guildTodayFlag[slot] != 0)
                {
                    addExp += static_cast<int>(
                        G_CDataManager()->m_guildParameter.m_fieldFC / 100.0 *
                            static_cast<double>(addExp));
                    user->add_guild_today_exp(addExp);
                }
                else
                {
                    user->add_guild_exp(addExp);
                }
            }
        }
        else
        {
            int lvl = user->get_charac_level();
            ref.m_field34 = static_cast<char>(lvl);
            unsigned int guildLvl = static_cast<unsigned int>(
                static_cast<unsigned short>(
                    static_cast<const STGuildDBInfoOnly*>(
                        user->GetGuildDBInfo())->m_memberCount));  // +0x22
            unsigned int tempExp =
                static_cast<unsigned int>(user->get_guild_temp_exp());
            // [R28a07 ④-精度轴] 同上（ORIG 0x085af4xx 第二处同构）。
            double weight = G_CDataManager()->m_guildParameter.getExpWeight(guildLvl);
            int addExp = static_cast<int>(
                static_cast<double>(weight) * static_cast<double>(tempExp));
            // ORIG:条件为 != 0(today 分支先发射,add_guild_exp 为 else)。
            if (guildTodayFlag[slot] != 0)
            {
                addExp += static_cast<int>(
                    G_CDataManager()->m_guildParameter.m_fieldFC / 100.0 *
                        static_cast<double>(addExp));
                user->add_guild_today_exp(addExp);
            }
            else
            {
                user->add_guild_exp(addExp);
            }
        }
        user->reset_guild_temp_exp();

        {
            // [R21-07 形态修复] 同第一处：ORIG 为链式调用（无 receiver 具名槽）。
            if (ARAD::Singleton<ServiceRestrictManager>::Get()->isRestricted(
                    RestrictCategory::Enum_3, 1))
            {
                ref.m_dropRate = user->getStdDropRate();
            }
        }

        {
            if (dropIt != result.m_dropItems.end())
            {
                rewardItems[0].setCopy(*dropIt);
                ++dropIt;
            }
        }

        if (m_battleData.m_arr54[slot] != 1 && m_battleData.m_arr58[slot] != 1 &&
            m_battleData.m_arr5c[slot] != 1)
        {
            bd.m_arr64[slot] += 1;
        }

        float deadPenalty = calDeadUserDungeonPenalty(slot);
        if (deadPenalty > 0.0f)
        {
            int addInfo = rewardItems[0].get_add_info();
            rewardItems[0].set_add_info(static_cast<int>(
                (1.0f - deadPenalty) * static_cast<float>(addInfo)));
            m_battleData.m_arr60[slot] = 1;
        }

        {
            int addInfo = rewardItems[0].get_add_info();
            rewardItems[0].set_add_info(static_cast<int>(
                // [R13a07 ④-G] ORIG 0x085aeee1 段：fildl(addInfo) 先压栈 →
                // fildl(minStdDropRate)/fldl(double 100.0)/fdivrp/fstps+flds(float 往返)
                // → fmulp ⇒ 原码 = `addInfo * (float)(minStdDropRate / 100.0)`
                // （double 除法后一次舍入到 float；原写 float 除法 + double 提升为形态/精度双差）。
                addInfo * static_cast<float>(minStdDropRate / 100.0)));
        }

        ref.m_field3c =                                          // +0x3c 整字视图
            user->isAffectedPremium(static_cast<ENUM_PREMIUM_TYPE>(0xc)) ? 3 : 0;
        WongWork::stGenerateResult_t genResult;
        GetLuckPoint(reinterpret_cast<CLuckPoint*>(&genResult.m_luckPoint), false);
        static_cast<WongWork::CClearReward*>(G_CDataManager()->getItemGenerator()->getGenerator(WongWork::CItemGeneratorMgr::eGenerateType_2))->generateRandomItem(ref, genResult);
        if (!genResult.m_dropItems.empty())
        {
            rewardItems[1].setCopy(*genResult.m_dropItems.begin());
        }

        ref.m_field3c =                                          // +0x3c 整字视图
            user->isAffectedPremium(static_cast<ENUM_PREMIUM_TYPE>(0xc)) ? 4 : 1;
        goldCardItems[0].setCopy(rewardItems[0]);
        goldCardItems[0].set_add_info(0);
        // ORIG：金卡组共用一条 getGenerator 链缓存（local_78）。
        WongWork::CClearReward* clearGen =
            static_cast<WongWork::CClearReward*>(G_CDataManager()->getItemGenerator()->getGenerator(
                WongWork::CItemGeneratorMgr::eGenerateType_2));

        bool goldCardPath = true;
        if (clearGen == 0)
        {
            goldCardPath = false;
        }
        else
        {
            CEventBase* ev10 = GlobalData::s_event_manager->GetRepeatEvent(0x10);
            if (ev10->IsEventing(static_cast<CUser*>(0)))
                goldCardPath = false;
        }
        if (goldCardPath)
        {
            float commissionRate = 1.0f;
            CEventBase* ev41 = GlobalData::s_event_manager->GetRepeatEvent(0x41);
            if (ev41->IsEventing(static_cast<CUser*>(0)) && IsPremiumGoldCardParty())
            {
                commissionRate = *reinterpret_cast<float*>(
                    &G_CDataManager()->m_serverParameter
                         .m_premiumGoldCard4894.m_field0);   // +0x48fc float 位型
            }
            CGameManager* gameMgr = G_CGameManager();
            CConditionEventManager* condMgr = gameMgr->GetConditionEventManager();
            if (condMgr != 0)
            {
                int step = user->GetCurConditionEventStep();
                if (condMgr->IsFreeGoldCard(user, static_cast<char>(step + 1)))
                    commissionRate = 0.0f;
            }
            int commission = clearGen->getGoldCardCommission(stdLevel);
            m_field1e4[1][slot] = static_cast<int>(
                static_cast<float>(commission) * commissionRate);
            float createRate = clearGen->getGoldCardCreateRate();
            // ORIG 0x085af2af：fstps -0x210(=ref+0x38) —— float 槽覆盖，非 +0x34
            ref.m_rankBonusRate = createRate;
        }

        genResult.m_dropItems.clear();
        {
            CEventBase* ev41 = GlobalData::s_event_manager->GetRepeatEvent(0x41);
            if (ev41->IsEventing(static_cast<CUser*>(0)) && IsPremiumGoldCardParty())
            {
                clearGen->generatePremiumGoldCardItem(
                    ref, genResult, static_cast<ENUM_RARITY>(-1),
                    GetPremiumGoldCardDefaultItem());
            }
            else
            {
                clearGen->generateRandomItem(ref, genResult);
                if (!genResult.m_dropItems.empty() &&
                    (IsReturnUserParty() || IsEventCharacParty()))
                {
                    clearGen->generatePremiumGoldCardItem(
                        ref, genResult, static_cast<ENUM_RARITY>(1), 10000);
                }
            }
        }

        // ORIG 0x085af461 区：非空 setCopy 代码物理在前、empty 的 reset+ev13 块
        // 在后（P59 布局方向）——源写非空先判形。
        if (!genResult.m_dropItems.empty())
        {
            goldCardItems[1].setCopy(*genResult.m_dropItems.begin());
            if (goldCardItems[1].m_addInfo == -1 ||
                m_battleField.m_pDungeon->m_89e != 0)
            {
                goldCardItems[1].reset();
            }
        }
        else
        {
            goldCardItems[1].reset();
            CEventBase* ev13 = GlobalData::s_event_manager->GetRepeatEvent(0x13);
            if (ev13->IsEventing(static_cast<CUser*>(0)))
            {
                clearGen->generateGoldCardBlankItem(goldCardItems[1]);
            }
        }

        if (m_battleField.m_pDungeon->m_villageAttackEventFlag != 0)
        {
            Inven_Item eventItem;
            eventItem.m_addInfo =
                G_CDataManager()->m_serverParameter.m_field4a4;  // +0x50c
            G_CDataManager()->m_itemList->create_item(
                static_cast<ENUM_ITEM_CREATE_TYPE>(1), eventItem, 1);
            goldCardItems[0].reset();
            goldCardItems[1].reset();
            rewardItems[0].reset();
            rewardItems[1].reset();
            rewardItems[1].setCopy(eventItem);
            bd.ResetEventDungeonClearPoint();
        }

        // ORIG 0x085af60d：mov -0xc8→-0x210(=ref+0x38) —— mapRatio 位型重写 float 槽
        ref.m_rankBonusRate = mapRatio;
        if (user->isAffectedPremium(static_cast<ENUM_PREMIUM_TYPE>(0xc)))
        {
            pcRoomItems[0].setCopy(rewardItems[0]);
            pcRoomItems[0].set_add_info(0);
            ref.m_field3c = 5;                                 // +0x3c 整字视图
            genResult.m_dropItems.clear();
            static_cast<WongWork::CClearReward*>(G_CDataManager()->getItemGenerator()->getGenerator(WongWork::CItemGeneratorMgr::eGenerateType_2))->generateRandomItem(ref, genResult);
            // ORIG 0x085af5xx 区：非空 setCopy 物理在前，empty 分支后置（P59）；
            // empty 内 reset 在 getGenerator 链之后。
            if (!genResult.m_dropItems.empty())
            {
                pcRoomItems[1].setCopy(*genResult.m_dropItems.begin());
            }
            else
            {
                WongWork::CClearReward* pcRoomGen =
                    static_cast<WongWork::CClearReward*>(G_CDataManager()->getItemGenerator()->getGenerator(
                        WongWork::CItemGeneratorMgr::eGenerateType_2));
                pcRoomItems[1].reset();
                if (pcRoomGen != 0)
                {
                    CEventBase* ev1d =
                        GlobalData::s_event_manager->GetRepeatEvent(0x1d);
                    if (ev1d->IsEventing(static_cast<CUser*>(0)))
                    {
                        pcRoomGen->generatePcRoomCardBlankItem(pcRoomItems[1]);
                    }
                }
            }
        }
    }

    unsigned int mobReward = 0;
    if (m_battleField.m_pDungeon->m_89e != 1)
    {
        G_CDataManager()->get_mob_reward(stdLevel, mobReward);
    }
    float partyBonusRate = spScript->m_expGroup368[memberCount - 1];
    partyBonusRate +=
        static_cast<CPartyExpBonusEvent*>(
            GlobalData::s_event_manager->GetRepeatEvent(0x18))
            ->getBonusRate(memberCount, isHelpAbuseParty());

    unsigned int totalMobExp = 0;
    if (m_battleField.m_pDungeon->m_89e != 1)
    {
        int killCount = bd.GetTotalKilledMonsterCount();
        // [R28a07 ④-精度轴] ORIG 0x085ae99e..0x085ae9b0：`mov $0x0,%edx; mov %eax,-0x310;
        //   mov %edx,-0x30c; fildll -0x310; fstps -0x33c`（= `(float)(unsigned)mobReward`
        //   **具名落盘**），随后 `fildl killCount; fmuls -0x33c` —— 原码 mobReward 先舍入为
        //   float（24bit 有效位）再乘；原写表达式内联使 4.4 保持 x87 80 位（少一次舍入，
        //   mobReward ≥ 2^24 时 baseExp 分叉）。P252 具名 float 局部强制落盘。
        float mw = static_cast<float>(mobReward);
        float baseExp = static_cast<float>(killCount) *
                        mw / 2.0f * partyBonusRate;
        float diffWeight =
            spScript->m_monsterExpTable398[bfView.get_dungeon_diff()];
        // [R28a07 ④-精度轴] ORIG 0x085af97d..0x085af99c：`flds 表项; flds -0x338(baseExp);
        //   fmulp; **fstps -0x334**; call get_exp_weight; fmuls -0x334` ⇒ 原码 weighted 是
        //   **float**（舍入点）；原写 `double weighted` 使 4.4 发 `fstpl`（double 舍入点），
        //   |weighted| > 2^24 时 totalMobExp 分叉。P252 同族。
        float weighted = baseExp * diffWeight;
        totalMobExp = static_cast<unsigned int>(static_cast<long long>(
            weighted * m_battleField.m_pDungeon->get_exp_weight()));
    }

    PacketGuard pkBasic;
    PacketGuard pkReward;
    std::list<map_item> questItemList;
    Inven_Item questItem;

    for (int slot = 0; slot <= 3; ++slot)
    {
        if (!_checkValidUser(slot))
        {
            continue;
        }
        user->setChattingMessageCount(0);

        if (m_battleField.m_pDungeon->m_89e != 0)
        {
            G_CDataManager()->get_mob_reward(user->get_charac_level(), mobReward);
            int killCount = bd.GetTotalKilledMonsterCount();
            // [R28a07 ④-精度轴] 同上（ORIG 0x085af3xx 第二处同构，fstps -0x324 具名落盘）。
            float mw = static_cast<float>(mobReward);
            float baseExp = static_cast<float>(killCount) *
                            mw / 2.0f * partyBonusRate;
            float diffWeight =
                spScript->m_monsterExpTable398[bfView.get_dungeon_diff()];
            // [R28a07 ④-精度轴] 同上（ORIG 0x085afaax 第二处同构，fstps -0x324）。
            float weighted = baseExp * diffWeight;
            totalMobExp = static_cast<unsigned int>(static_cast<long long>(
                weighted * m_battleField.m_pDungeon->get_exp_weight()));
        }

        int userLevel = user->get_charac_level();
        unsigned int personalExp = 0;
        if (m_battleField.m_pDungeon->m_89e == 1)
        {
            // ORIG：get_index()==100 与否两分支体相同（退化分支，忠实保留）。
            if (m_battleField.m_pDungeon->get_index() == 100)
            {
                personalExp = totalMobExp / memberCount;
            }
            else
            {
                personalExp = totalMobExp / memberCount;
            }
        }
        // ORIG:条件为 isRisk(MPR-only 分支先发射)。
        else if (m_battleField.m_pDungeon->isRiskDungeon())
        {
            // [R5a07 ④] ORIG 0x085afcb8..0x085afd21：`mov $0x0,%edx; fildll -0x310;
            //   fstps -0x324` ⇒ **先把 (unsigned)totalMobExp 截断为 float**，随后
            //   `fildl MPR; flds -0x324; fmulp; fildl -0xe0; fdivrp; fistpll`（乘除均在扩展
            //   精度、**无外层 float 截断**）。原写 `(float)((double)(unsigned long long)x * m)`
            //   ⇒ x 未先舍入（x>2^24 时 fstps 舍入点不同，fistpll 可差 1）。
            personalExp = static_cast<unsigned int>(static_cast<long long>(
                static_cast<float>(totalMobExp) * MemberPenaltyRate() / memberCount));
        }
        else
        {
            // [R5a07 ④] ORIG 0x085afcca..0x085afd33：`flds -0x320; fmulp; fstps -0x31c`
            //   ⇒ penalty × (float)(unsigned)totalMobExp 后**截断为 float**（具名 float 槽），
            //   再 `fildl MPR; flds -0x31c; fmulp; fildl -0xe0; fdivrp; fistpll`。
            float scaled = static_cast<float>(
                static_cast<float>(totalMobExp) *
                G_CDataManager()->BaseExpPenalty(userLevel, stdLevel));
            // [R13a07 ④-G] ORIG 0x085afcef/0x085afcf5：`flds -0x31c; fmulp`（float 直接读回，
            // 无 `fstpl -0x3c8; fldl` 中转）⇒ 原码无 `(double)scaled` 提升（float→double 无损，
            // 数值等价，但本轮要求逐条复刻 ORIG 形态）。
            personalExp = static_cast<unsigned int>(static_cast<long long>(
                scaled * MemberPenaltyRate() / memberCount));
        }

        QuickParty::RewardData_Param rewardParam(this);
        QuickParty::RewardData_Result rewardResult;
        rewardResult.m_value00 = static_cast<int>(personalExp);
        set_random_reward(static_cast<QuickParty::RandomBuffType>(0), rewardParam,
                          rewardResult);
        personalExp = static_cast<unsigned int>(rewardResult.m_value00);

        stClearRewardExpParameter_t expParam;
        expParam.m_deadMemberCount = slot;
        expParam.m_rankBonusIdx = rankBonusIdx;
        set_clear_reward_exp_parameter(user, personalExp, expParam);
        stClearRewardBonusExp_t bonus;
        unsigned int bonusTotal =
            getClearRewardBonusExp(user, personalExp, expParam, bonus);

        bool helpAbuse = isHelpAbuseParty(user);
        user->calcHelpAbuseRatio(bonusTotal, helpAbuse);
        user->reset_level_before_dungeon();

        int spPoints = 0;
        int sfpPoints = 0;
        bool leveledUp = user->gain_exp_sp(
            static_cast<int>(bonusTotal), spPoints, sfpPoints,
            static_cast<CUser::eExpAddReason>(2), 0, false);
        if (leveledUp)
        {
            pkBasic.clear();
            pkBasic.put_header(0, 2);
            pkBasic.put_byte(1);
            pkBasic.put_short(1);
            user->make_basic_info(reinterpret_cast<char*>(&pkBasic), 1);
            pkBasic.finalize(true);
            user->Send(pkBasic);
        }

        if (user->IsHavePremiumAdvantage())
        {
            float premiumDeduction;   // ORIG quirk: flds -0x2cc has no writer
            int deduction = static_cast<int>(premiumDeduction);
            user->m_character.m_f28 -= deduction;
            user->m_character.m_f34 -= deduction;
        }

        questItemList.clear();

        pkReward.clear();
        pkReward.put_header(0, 0x23);
        pkReward.put_int(bonus.m_channelBonusExp + static_cast<int>(personalExp));
        pkReward.put_int(bonus.m_rankWeightExp);
        user->set_charac_party_bonus_exp(static_cast<int>(bonus.m_rankWeightExp));

        unsigned int extraExp = 0;
        {
            // ORIG 0x085b0249 区：分母 = 表值重读(fVar1) + 新算 bonusRate(lVar20)。
            // partyBonusRate 局部已含首轮 bonusRate，不能再用它作基数再加。
            // [R27-07 ④] ORIG 0x085b0250 段三条：`fildll -0x310`（personalExp 零扩展 64 位）
            // → `fstps -0x318`（(float)personalExp **单精度落具名槽**）
            // → 末尾 `fdivrs -0x318`（ST(0) = mem/ST(0)）。我方原写内联
            // `static_cast<float>(personalExp) / (bonusRate + tableRate)` ⇒ 4.4 保持 x87
            // 扩展精度（fildll → fdivrp，无 fstps 舍入）⇒ personalExp ≥ 2^24 时有效位被
            // ORIG 舍掉而我方保留，商不同（最小反例 personalExp=16777217、分母 1.5：
            // ORIG 11184810 vs 我方 11184811）⇒ extraExp 差 (sum/personalExp+1) 倍。
            // 具名 float 局部强制落槽舍入（P252），复现 ORIG 三条同形。
            float numerator = static_cast<float>(personalExp);
            float tableRate = spScript->m_expGroup368[memberCount - 1];
            float bonusRate =
                static_cast<CPartyExpBonusEvent*>(
                    GlobalData::s_event_manager->GetRepeatEvent(0x18))
                    ->getBonusRate(memberCount, isHelpAbuseParty());
            if (personalExp != 0)
            {
                int sub = static_cast<int>(
                    numerator / (bonusRate + tableRate));
                unsigned int sum = bonus.m_avatarBonusExp +
                                   bonus.m_rankWeightExp +
                                   bonus.m_selectedBonusExp +
                                   bonus.m_channelBonusExp +
                                   bonus.m_premiumAdvExp +
                                   bonus.m_serverEventExp +
                                   bonus.m_serverChannelExp +
                                   bonus.m_creatureBonusExp;
                extraExp = (personalExp - static_cast<unsigned int>(sub)) *
                           (sum / personalExp + 1);
            }
        }
        pkReward.put_int(static_cast<int>(extraExp));
        pkReward.put_int(bonus.m_avatarBonusExp);
        pkReward.put_byte(bonus.m_bestType);
        pkReward.put_int(bonus.m_selectedBonusExp + bonus.m_serverEventExp +
                         bonus.m_burningFatigueExp);
        pkReward.put_int(bonus.m_periodItemExp);
        pkReward.put_int(bonus.m_premiumAdvExp);
        pkReward.put_int(bonus.m_pcRoomSameUserExp);
        pkReward.put_int(bonus.m_serverChannelExp);
        pkReward.put_int(bonus.m_upperMemberExp);
        pkReward.put_int(bonus.m_creatureBonusExp);
        pkReward.put_int(bonus.m_returnUserExp);
        pkReward.put_int(bonus.m_eventCharacExp);
        pkReward.put_int(bonus.m_manageExp);
        pkReward.put_int(bonus.m_growthPowerExp);
        pkReward.put_int(bonus.m_premiumAddExp);
        int questCountPos = pkReward.get_index();
        pkReward.put_byte(0);

        int questPutCount = 0;
        for (int qi = 0; qi < 0x14; ++qi)
        {
            UserQuest* questRef = user->getCurCharacQuestRefR();
            if (questRef->m_quest[qi] == 0 || questRef->m_trigger[qi] == 0)
            {
                continue;
            }
            Quest* quest = G_CDataManager()->find_quest(questRef->m_quest[qi]);
            if (quest == 0)
            {
                continue;
            }
            if (!quest->check_clear_item(bfView.get_dungeon_index(),
                                         static_cast<int>(
                                             questRef->m_authenData.m_characClass)))
            {
                continue;
            }
            int requiredCount = questRef->m_quest[qi];
            if (requiredCount <= 0)
            {
                continue;
            }
            // ORIG：默认构造 vector + pair，再由 get_clear_itemlist 返回值赋值。
            std::vector<std::pair<int, int> > clearItems;
            std::pair<int, int> entry;
            clearItems = quest->get_clear_itemlist(bfView.get_dungeon_index());
            // [轴 B，地址自校 0x085b0907] ORIG `addl $1,-0x30; call <size>;
            // cmp -0x30,%eax; setg %al` = **有符号**比较（我方旧文 unsigned
            // 产 `seta`，符号性相反）⇒ 以 ORIG 为准改 int + (int)size()。
            for (int ci = 0; ci < (int)clearItems.size(); ++ci)
            {
                questItem.reset();
                entry = clearItems.at(ci);
                // [R28-04 §34.2.2 真④] ORIG 0x085b0698/0x085b069e：
                //   `mov -0x184(%ebp),%eax; mov %eax,-0x1fb(%ebp)`
                //   = questItem(+0x02 m_addInfo) = entry.first（本次要造的 itemId）。
                //   消费链（ORIG）：CItemList::create_item @0x08511fcd 先
                //   `set_add_info(count)` 写 +0x07，随后 0x08511fd5
                //   `mov 0x2(%eax),%eax` + 0x08511fe2 `find_item` 取 +0x02 作 itemId
                //   ⇒ 缺此写则 find_item(未初始化/0) 走建物失败路径。
                questItem.m_addInfo = entry.first;
                G_CDataManager()->m_itemList->create_item(
                    static_cast<ENUM_ITEM_CREATE_TYPE>(1), questItem,
                    entry.second);
                CInventory* inventory = user->getCurCharacInvenW();
                int putSlotNo = inventory->insertItemIntoInventory(
                    questItem, static_cast<eItemAddReason>(0xb), true, true);
                if (putSlotNo >= 0)
                {
                    pkReward.put_short(putSlotNo);
                    pkReward.put_int(entry.first);
                    pkReward.put_int(questItem.get_add_info());
                    pkReward.put_short(static_cast<int>(questItem.m_fieldb));
                    pkReward.put_byte(static_cast<int>(
                        static_cast<unsigned char>(
                            questItem.m_amp.getAbilityType())));
                    pkReward.put_short(
                        static_cast<int>(questItem.m_amp.getAbilityValue()));
                    questPutCount += 1;
                    if (questItem.m_field1 == 1)
                    {
                        CItem* itemDef = G_CDataManager()->find_item(entry.first);
                        if (itemDef != 0)
                        {
                            CLuckPoint::UseLuckPoint(
                                user,
                                static_cast<int>(questItem.m_field1),
                                itemDef->get_rarity(),
                                itemDef->get_grade());
                        }
                    }
                }
            }
        }
        if (questPutCount != 0)
        {
            pkReward.put_byte(questCountPos, questPutCount);
        }

        for (int j = 0; j <= 3; ++j)
        {
            if (_checkValidUser(j))
            {
                _makeRewardItemInfo(m_member[j].m_pUser, bd.m_itemGroups0[j],
                                    pkReward);
            }
            else
            {
                pkReward.put_byte(0);
            }
        }
        pkReward.put_int(m_field1e4[1][slot]);

        for (int j = 0; j <= 3; ++j)
        {
            // ORIG：两分支体相同（各 put_byte(0)，退化分支，忠实保留）。
            if (_checkValidUser(j))
            {
                pkReward.put_byte(0);
            }
            else
            {
                pkReward.put_byte(0);
            }
        }

        for (int j = 0; j <= 3; ++j)
        {
            if (_checkValidUser(j))
            {
                if (!m_member[j].m_pUser->isAffectedPremium(
                        static_cast<ENUM_PREMIUM_TYPE>(0xc)))
                {
                    pkReward.put_byte(0);
                }
                else
                {
                    _makeRewardItemInfo(m_member[j].m_pUser,
                                        bd.m_itemGroups1[j], pkReward);
                }
            }
            else
            {
                pkReward.put_byte(0);
            }
        }

        // ORIG clear_reward.md +0x2aa2..+0x2b00：flds 100.0 × hellValue ÷
        // fildl(int m_hellPartyRatio0) 后 fldcw RC=11 纯截断（无 +0.5f fadd）。
        int hellCoinValue = static_cast<int>(
            100.0f * bd.m_hellPartyValueTotal /
                static_cast<float>(bfView.m_hellPartyRatio0));
        pkReward.put_int(hellCoinValue);

        bool returnFlag = false;
        if (user->GetParty() != 0)
        {
            returnFlag =
                reinterpret_cast<CParty*>(user->GetParty())->IsReturnUserParty();
        }
        // ORIG:if/else 两分支各一次 put_byte 调用(静态双站点)。
        if (returnFlag)
        {
            pkReward.put_byte(1);
        }
        else
        {
            pkReward.put_byte(0);
        }

        bool eventFlag = false;
        if (user->GetParty() != 0)
        {
            eventFlag =
                reinterpret_cast<CParty*>(user->GetParty())->IsEventCharacParty();
        }
        if (eventFlag)
        {
            pkReward.put_byte(1);
        }
        else
        {
            pkReward.put_byte(0);
        }

        if (expParam.m_deadPenaltyRate != 0.0f)
        {
            pkReward.put_byte(1);
        }
        else
        {
            pkReward.put_byte(0);
        }

        pkReward.finalize(true);
        user->Send(pkReward);

        if (m_battleField.m_pDungeon != 0 && user != 0 && m_battleField.m_bloodState != 1 &&
            user->getCurCharacR() != 0)
        {
            bool properDungeon = m_battleField.m_pDungeon->m_bloodDungeonType == 0 &&
                                 m_battleField.m_pDungeon->get_dimension_possible() == 0 &&
                                 m_battleField.m_pDungeon->m_89e != 1 &&
                                 !m_battleField.m_pDungeon->isTowerOfDespairDungeon();
            if (properDungeon && user->IsProperDungeonUser())
            {
                HeroMissionEvent* heroMission =
                    ARAD::Singleton<HeroMissionEvent>::Get();
                heroMission->processMission(
                    user, static_cast<HeroMissionCondition::MissionNo::T>(1), 1);
                user->IncProperDungeonClearCount();
                CGameManager* gameMgr2 = G_CGameManager();
                CConditionEventManager* condMgr2 =
                    gameMgr2->GetConditionEventManager();
                if (condMgr2 != 0)
                {
                    condMgr2->ProcessCheckStepUp(user, 2);
                }
            }
        }
        if (m_battleField.m_pDungeon != 0 && user != 0 && m_battleField.m_bloodState != 1 &&
            user->getCurCharacR() != 0)
        {
            bool properLevel = m_battleField.m_pDungeon->m_bloodDungeonType == 0 &&
                               m_battleField.m_pDungeon->m_89e != 1 &&
                               !m_battleField.m_pDungeon->isTowerOfDespairDungeon();
            if (properLevel && user->IsProperLevelDungeonUser())
            {
                user->ProcProperLevelDungeonClear();
            }
        }
    }
#undef user
#undef rewardItems
#undef goldCardItems
#undef pcRoomItems
#undef bd
#undef bfView

    if (IsPremiumGoldCardParty())
    {
        PacketGuard goldCardPacket;
        goldCardPacket.put_header(0, 0x12a);
        goldCardPacket.finalize(true);
        send_to_party(goldCardPacket);
    }
    if (m_battleField.m_pDungeon->m_entryRestrictFlag == 0)
    {
        DungeonPermission(param);
    }
}