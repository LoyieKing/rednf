// ---- _generateRandomItem — ORIG T 0x08535ed2 ----
// [b28d-solo-WongWorkDrop_A] verdict=DIFF(N82/N78 封顶)：p167 CLEAR 50=50
//（0 missing/0 extra）；gap=19 全为跨版本 codegen——N78（ORIG altTable 清零
// rep stos 对齐剥离慢路 vs 4.4 简单形）、N82（帧 0x24c vs 0x25c：ORIG buf
// 与 goldItem 同址复用，全帧槽号 +0x10 平移）、N85（x87 fdivs/fucompp-fnsahf
// vs fdivp/fucomip）。真语义 bug 修 6 处（见下）。
// [round4 L3CUser 复核 2026-09-10] 帧 0x24c vs 我方 0x25c（+0x10）、槽号
//   -0x21c/-0x1fc/-0x204/-0x208 vs -0x22c/-0x20c/-0x214/-0x218 恒定平移 ⇒
//   判据⑪ B-FRAME（整帧平移）⇒ N126/N82，声明重排无效；gap -19 为此族固有。
// [A28 round5 复核 2026-09-10] 重跑 p167：CLEAR 50=50（仅 Inven_Item C1↔C2 同址
//   别名，P93 良性）；orig 665 / ours 646（raw 670/652）；帧 0x24c vs 0x25c。
//   ⇒ 维持 N78（altTable `rep stos` 对齐剥离慢路）+ N82/N126（整帧 +0x10）封顶。
// [M1S5 2026-09-11 新钥匙逐条回扫 ⇒ 未推翻]
//   ①p167 复跑 CLEAN：ORIG calls=50 / OUR calls=50，唯一差集 = `Inven_ItemC2Ev`×2
//     vs `C1Ev`×2（同址别名 P93，本函数两处 `Inven_Item x;` 完整对象构造）；
//   ②§6.6.2 P216 不适用：两侧 `_Unwind_Resume`/`_ZdlPv`/`__cxa_begin_catch`
//     计数全为 0；③ORIG 独有段仅 1 处 `rep stos`（altTable 清零慢路，N78；
//     P312 形 `char buf[256]={0}` 已于 round5 实测反劣 +34 并回滚 ⇒ 维持）；
//   ④P339 不适用：槽集合非「槽映射互换」而是**混合区块平移** —— 深区
//     ORIG 0x1fc/0x200/0x204/0x208/0x20c/0x20e/0x210/0x214/0x218/0x21c vs
//     我方 0x20c/0x210/0x214/0x218/0x21c/0x21e/0x220/0x224/0x228/0x22c（+0x10），
//     而 buf/-0x1eb.. 区 +0xc、浅区 0x28/0x2c↔0x20/0x24 与 0xb0↔0xbc 各自独立
//     （+0x8/-0xc）⇒ 根因是块内聚合(Inven_Item/char[256]/int[6])与标量的
//     4.4 分区规则，属 P339 失效域②；gap −19 = N78(rep stos) + 帧差固有。
// 参数（自 wrapper）【推断命名】：
//   type       = ref+0x34 怪物等级档
//   role       = ref+0x35（signed char；==3 走首领金币路径）
//   dungeonLv  = ref+0x36 地下城等级档
//   difficulty = ref+0x37 难度档
//   partyPos   = ref+0x38 组队档位（-1 后作加成列下标）
//   context3c  = *(int*)&ref[0x3c]（人数上下文，clip [0,100]）
//   dropRate   = ref.m_dropRate（×value/100）
// [P6AuditOldN.P6S2 2026-09-11 · 本波复核] n1_v DIFF 670/652 gap=+18（blocks=114）。
//   ORIG 0x08535ed2：`sub $0x24c,%esp; mov %eax,-0x21c(%ebp)` vs 我方
//   `sub $0x25c,%esp; mov %eax,-0x22c(%ebp)`（帧差 0x10 + 槽平移 0x10）。
//   新钥匙判定：P339（非单槽序问题）/P0 删具名局部（缺 +0x10 槽的对应证据）/
//   类型轴三式（无掩码/cast 形）均不适用；未落地，维持 N（未决）。
// [Q8 2026-09-11 旧N判词重扫] 旧判=DIFF（N78/N82/N126）→ 试=新口径 n1_v 670/652 gap=+18 blocks=114（与 P6S2 复核一致）；P339 非单槽序（块内聚 Inven_Item/char[256]/int[6] 与标量分区）、P0 无 +0x10 槽证据、类型轴无掩码/cast 形 ⇒ 维持 N（帧 0x24c vs 0x25c 整帧平移）。
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
}