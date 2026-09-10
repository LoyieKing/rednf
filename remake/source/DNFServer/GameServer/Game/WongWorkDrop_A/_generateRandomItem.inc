void CMonsterDrop::_generateRandomItem(unsigned char type, char role,
                                       unsigned char dungeonLv,
                                       unsigned char difficulty,
                                       unsigned char partyPos, int context3c,
                                       int dropRate,
                                       stGenerateResult_t& result, bool& done)
{
    // [b28d-solo-WongWorkDrop_A] ORIG 首参取第 3 形参 dungeonLv（-0x204 槽）；
    // 权重行一次 operator[] 后 7 个连续 mov（结构整体拷贝 -0x90..-0x78）。
    int idx = _getGenItemProbIdx(dungeonLv);
    if (idx < 0)
        return;

    stGenItemProb_t t = m_probsSub[idx];

    unsigned char partyCol = static_cast<unsigned char>(partyPos - 1);

    // 基础概率（ORIG fildl t.v[2] 先入栈，再 flds dungeon/monster/party 逐次
    // fmulp；dungeonBonus 索引用第 4 形参 difficulty）
    int chance = static_cast<int>(
        static_cast<float>(t.v[2]) *
        *reinterpret_cast<float const*>(&m_dungeonBonus[0][0] + difficulty) *
        *reinterpret_cast<float const*>(&m_monsterTypeBonus[0][0] + role) *
        *reinterpret_cast<float const*>(&m_partyBonus[0][0] + partyCol));

    // member_cnt clip：max(context,0)→min(100)；>50 打递减折扣
    context3c = std::min<int>(100, std::max<int>(0, context3c));
    if (context3c > 50)
    {
        float decay = 0.01f + 0.002f * static_cast<float>(context3c - 50);
        chance = static_cast<int>((1.0f - decay) * static_cast<float>(chance));
    }
    chance = (chance * dropRate) / 100;                // imul/sar 除法

    // ORIG：setl 不成立时**跳过金币段**继续 done 检查（非 early return）
    if (m_random.generateNumber(0, 10000) < chance)
    {
        // 金币（m_common->getGoldGenTable(type)：+0 基准 / +4 浮动幅度）
        int* gt = m_common->getGoldGenTable(type);
        int goldAmt = gt[0] + (gt[0] * GetRandomRange(-gt[1], gt[1])) / 100;

        Inven_Item goldItem;
        goldItem.m_field1 = static_cast<unsigned char>(2); // 字节 +0x01=金币类型
        goldItem.m_addInfo = 0;                            // uStack_1ef 清零
        goldItem.set_add_info(std::max<int>(1, goldAmt));  // ORIG 无条件先设一次

        if (role == 3)
        {
            unsigned short cnt = 0;                        // local_9a
            unsigned short volume = 0;                     // local_9c
            if (!_getBossGoldDrop(partyCol, cnt, volume))
            {
                result.m_dropItems.push_back(goldItem);
            }
            else
            {
                goldAmt = (static_cast<int>(volume) * goldAmt) / 100;
                goldItem.set_add_info(std::max<int>(1, goldAmt));
                for (int i = 0; i < static_cast<int>(cnt); ++i)
                    result.m_dropItems.push_back(goldItem);
            }
        }
        else
        {
            result.m_dropItems.push_back(goldItem);
        }
    }

    if (done)                                          // *(param_9) 非 0 → 结束
        return;

    // ---- 稀有装备主循环 tier 1..4 ----
    for (int tier = 1; tier <= 4; ++tier)
    {
        float marketFactor = 1.0f;                     // local_50（tier==2 被 rate 覆盖）
        int altTable[6] = {0};                         // ORIG rep stos 对齐剥离慢路

        int* rarityTable = &m_rarityBasis[tier][0];    // this + tier*0x18 + 0x30
        if (tier == 2)
        {
            // ORIG 两次独立 G_CDataManager()->GetAutoMarketContitionsControl()；
            // ChangeItemRarity 第 4 实参是立即数 6（非 tier，P97）
            marketFactor =
                G_CDataManager()->GetAutoMarketContitionsControl()->GetItemDropRate(
                    idx);
            double extra = 0.0;
            if (marketFactor != 0.0f)
                extra = 1.0 / marketFactor - 1.0;
            if (G_CDataManager()
                    ->GetAutoMarketContitionsControl()
                    ->ChangeItemRarity(&m_rarityBasis[tier][0], altTable, 6,
                                       extra))
                rarityTable = altTable;
        }

        float weight = static_cast<float>(t.v[2 + tier]);
        if (tier == 2)
            weight *= static_cast<float>(m_firstMobWeight) * m_firstMobWeightRate;

        int rollChance = static_cast<int>(
            (*reinterpret_cast<float const*>(&m_partyBonus[tier][0] + partyCol) *
             *reinterpret_cast<float const*>(&m_monsterTypeBonus[tier][0] + role) *
             *reinterpret_cast<float const*>(&m_dungeonBonus[tier][0] + difficulty) *
             weight * marketFactor)) * dropRate / 100;

        int randValue = m_random.generateNumber(0, 10000);
        if (rollChance < randValue)
            continue;

        int rarRoll = m_random.generateNumber(1, 1000000);
        int rarity = reinterpret_cast< ::CLuckPoint*>(&result.m_luckPoint)
                         ->GetItemRarity(rarityTable, rarRoll, tier);

        G_CDataManager()->getItemGenerator()->onTryGenerateRandomItem(
            static_cast<CItemGeneratorMgr::eGenerateType_t>(getGeneratorType()),
            static_cast<eItemDropType_t>(tier),
            static_cast<ENUM_RARITY>(rarity));

        int itemIdx =
            m_table.chooseItem(static_cast<eItemDropType_t>(tier),
                               static_cast<ENUM_RARITY>(rarity), type);
        if (itemIdx == -1)
            continue;

        if (tier == 2)
        {
            // [A28 round5 · P312 实测反例 2026-09-10 19:55] 曾按 ORIG 0x085360xx 段
            //   （-0x1ed 起 0x100 字节对齐剥离慢路 rep stos）把本数组改为
            //   `char buf[256] = {0};`：**反劣**（ours 646→680，+34；4.4 的零初始化
            //   形与本处槽位/帧配合后长于 ORIG）⇒ 已回滚；维持无初始化形并封 N78。
            char buf[256];                             // -0xed 起 0x100 日志缓冲
            sprintf(buf,
                    "monster drop rate : item_id(%d), mon_lv(%d), role(%d), "
                    "dungeon_lv(%d), dungeon_diff(%d), member_cnt(%d), "
                    "bourgeois_rate(%d), drop_rate(%d), rand_value(%d), "
                    "drop_value(%d)",
                    itemIdx, static_cast<int>(type), static_cast<int>(role),
                    static_cast<int>(dungeonLv),
                    static_cast<int>(difficulty),
                    static_cast<int>(partyCol), context3c, dropRate, randValue,
                    rollChance);
            reinterpret_cast< ::CLuckPoint*>(&result.m_luckPoint)
                ->sendItemDropRate(buf);
        }

        CItem* item = G_CDataManager()->m_itemList->find_item(itemIdx);
        if (item == 0)
            continue;

        Inven_Item equip;                              // local_f1 缓冲
        equip.m_addInfo = itemIdx;
        G_CDataManager()->m_itemList->create_item(
            static_cast<ENUM_ITEM_CREATE_TYPE>(1), equip, 1);

        if (tier == 2)
        {
            // 非宠物(vtbl+0x14)且非头像(vtbl+0x10)才进入装备升档分支
            if (!item->IsCreatureItem() && !item->IsAvatarItem())
            {
                int grade = item->get_grade();
                equip.set_add_info(
                    m_upgrade.generateEquipGrade(type - grade));
            }
            // [b28d-solo-WongWorkDrop_A] 真语义 bug 修正：ORIG 0x0853676f
            // `test %al,%al; je <false>` + `hasAbility xor $1; je <false>` ⇒
            // 进入条件 = checkItemUpgradePossible()!=0 && hasAbility()!=1
            //（旧码写反为 !check && !hasAbility，极性整段互换）
            if (item->checkItemUpgradePossible() && !equip.m_amp.hasAbility())
            {
                int grade = item->get_grade();
                int upLv = m_upgrade.generateUpgrade(
                    static_cast<ENUM_RARITY>(item->get_rarity()), type - grade);
                if (upLv > 0)
                    equip.SetUpgrade(static_cast<unsigned char>(upLv));
            }
        }

        // ORIG 无 seq 局部：size() 结果留 %esi 跨 get_grade/get_rarity 保活
        reinterpret_cast< ::CLuckPoint*>(&result.m_luckPoint)
            ->UseLuckPoint(static_cast<int>(equip.m_field1),
                           item->get_rarity(), item->get_grade(),
                           static_cast<int>(result.m_dropItems.size()));
        result.m_dropItems.push_back(equip);
    }
