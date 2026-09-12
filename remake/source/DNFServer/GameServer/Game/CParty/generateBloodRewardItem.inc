// ============================================================================
// CParty::generateBloodRewardItem —— ORIG 0x85b840e：血轮物品奖励
// （rand<1000 分稀有/普通两生成路径，终局轮追加终极奖励）。
// ============================================================================
// [SweepCParty] 真 bug/缺口：find_item 目标由 CDataManager 改 CItemList（ORIG
// `CItemList::find_item(G_CDataManager()+0xc)`）；ref 初始化 memcpy→逐字段赋值；
// 去 dm 缓存（ORIG 两次独立 call G_CDataManager）。
// b28d-solo-CParty2：迭代器 begin/de 按 ORIG 6/6 对账修正（rare 段首处内联 begin
// 表达式 + 非 const `it` 复用两次 de；normal 段恢复第二对 begin/de）⇒ p167 只剩
// C1/C2、D1/D2 名族与 CLuckPoint EXTRA（判 N/P204：删独立局部会少发射 12 个 ORIG
// 同名 W 副本，账面回退）。ndiff 360→328。
void CParty::generateBloodRewardItem(int slot, int round, unsigned int param1,
                                     unsigned int param2, bool flag)
{
    // b28d-49 判词：N 封顶 + 缺口。P167 MISS G_CDataManager + 非 const iterator de/begin +
    // CItemList::find_item 4 项 / EXTRA CDataManager::find_item + memcpy + CLuckPoint 临时。@0x85b840e
    // P183：ORIG 各使用点重取址（无引用缓存槽）⇒ 宏直用。
    // [G7-sub] 本 .inc 域内**无任何语句**（仅函数头 + 判词；主体在聚合器 cpp，非本泳道写域）
    // ⇒ 无法在本轮改动。读数 495(ORIG)/484(OURS)：缺口 11 = P167 MISS 调用项
    // （G_CDataManager 重调 / CItemList::find_item / 迭代器 de+begin），属 cpp 侧待补。
#define bf (*reinterpret_cast<CBattle_Field*>(&m_battleField))
    bf.m_bloodReward.setRewardItem(slot, round);
    for (int i = 0; i < round; ++i)                     // -0x3c
    {
        WongWork::stGenerateRefData_t ref;              // -0x198
        WongWork::stGenerateResult_t result;            // -0xa4
        CLuckPoint luckPoint;                           // -0x98
        // [SweepCSecurityCard 2026-09-10] 已试形（回滚）：ORIG 0x085b8472
        // `lea -0xa0(%ebp),%eax; add $0xc,%eax` 指向 result.m_luckPoint
        //（stGenerateResult_t 内嵌 ::CLuckPoint@+0x0c）；改 `GetLuckPoint(&result.m_luckPoint,…)`
        // 删独立局部后 p167 EXTRA 消失，但本 TU 少发射 12 个 ORIG 同名 W 副本
        // ⇒ compare 四数 147/323→144/314 账面回退，按验收「不回退」回滚，判 N/P204。
        GetLuckPoint(&luckPoint, false);
        unsigned int roll = (unsigned int)get_rand_int(1000);   // -0x38
        // ORIG generateBloodRewardItem.md 085b849e `mov 0x6154(%eax)` /
        // 085b84b0 `mov 0x6158(%eax)`（int）：0x6154/58 =
        // CDataManager::m_bloodDungeon(+0x5e30)+0x324/0x328 →
        // stBloodDungeon_t::m_field324[0]/[1]（int，稀有/普通掉落基数）
        // P0/P183：ORIG 0x085b8499/085b84ab 两次独立 call G_CDataManager（无 dm 缓存）。
        unsigned int rareLine =
            G_CDataManager()->m_bloodDungeon.m_field324[0] * param2;    // -0x34
        unsigned int normalLine =
            G_CDataManager()->m_bloodDungeon.m_field324[1] * param2;    // -0x30

        if (roll < rareLine)
        {
            // ORIG 0x085b84c9..0x085b8505 逐条：+0x35=0、+0x34=param1、+0x36=1、
            // +0x38=movl 1、+0x3c/+0x3e/+0x40/+0x42=movw 1（无 memcpy）。
            ref.m_field35 = 0;                          // +0x35
            ref.m_field34 = (char)param1;               // +0x34
            ref.m_rateSlot = 1;                         // +0x36
            ref.m_rankBonusIndex = 1;                   // +0x38
            ref.m_goldBase = 1;                         // +0x3c
            ref.m_goldFactor = 1;                       // +0x3e
            ref.m_field40 = 1;                          // +0x40
            ref.m_field42 = 1;                          // +0x42

            WongWork::stGenerateResult_t specific;      // -0xd4
            WongWork::CItemGeneratorMgr* genMgr =
                G_CDataManager()->getItemGenerator();
            genMgr->getGenerator((WongWork::CItemGeneratorMgr::eGenerateType_t)2)
                ->generateSpecificItem(ref, specific);
            // b28d-solo-CParty2（p167 对账）：ORIG rare 段 begin/de = (0x85b8569,8577)
            // + (0x85b8643,8674) + 0x85b86ad ⇒ 首处内联 begin 表达式（无 item 引用槽），
            // 后续用**非 const 迭代器** `it`（ORIG 的 operator* 为
            // `_ZNK...IP10Inven_Item...deEv` 非 const 版，两次 de 复用同一迭代器）。
            bf.m_bloodReward.setRewarItemCopy(
                *specific.m_dropItems.begin(), slot, i);

            int dungIdx =
                m_battleField.m_pDungeon != 0 ? m_battleField.m_pDungeon->get_index() : 0;   // -0x2c
            std::vector<Inven_Item>::iterator it = specific.m_dropItems.begin();
            QuickParty::RewardData_Param rp(this);
            rp.m_param4 = dungIdx;
            QuickParty::RewardData_Result rr;
            // [SweepCSecurityCard 2026-09-10] ORIG 0x085b8679 `mov 0x7(%eax),%eax`
            // 直读 +0x07（Inven_Item::m_addInfo2），无 get_add_info out-of-line
            // call（p167 EXTRA get_add_info ×1）。
            rr.m_value00 = (*it).m_addInfo2;
            set_basic_reward((QuickParty::BasicRewardType)0xb, rp, rr);
            Inven_Item& mutable_item = const_cast<Inven_Item&>(*it);
            mutable_item.m_addInfo2 = rr.m_value00;
            // ORIG 085b86bd `mov 0x6160(%eax)`：m_bloodDungeon+0x330 →
            // stBloodDungeon_t::m_field330（float，flds 消费同宽）
            float addRate = G_CDataManager()->m_bloodDungeon.m_field330;
            bf.m_bloodReward.setRewardItemAddInfo(addRate, slot, i);
        }
        else if (roll < normalLine)
        {
            // ORIG 0x085b8720..0x085b874a 逐条：+0x35=0、+0x34=param1、
            // +0x38=movl 0x3f800000（1.0f 位型）、+0x40=指针整字、+0x3c=movl 0。
            ref.m_field35 = 0;
            ref.m_field34 = (char)param1;
            ref.m_rankBonusIndex = 0x3f800000;          // +0x38
            ref.m_dungeon = m_battleField.m_pDungeon;   // +0x40
            ref.m_field3c = 0;                          // +0x3c

            WongWork::CItemGeneratorMgr* genMgr =
                G_CDataManager()->getItemGenerator();
            genMgr->getGenerator((WongWork::CItemGeneratorMgr::eGenerateType_t)2)
                ->generateRandomItem(ref, result);
            if (!result.m_dropItems.empty())
            {
                const Inven_Item& item = *result.m_dropItems.begin();
                // b28d-solo-CParty2（p167 对账）：ORIG normal 段 begin/de =
                // (0x85b87ba,87c8) + (0x85b886c,887a) 两对 ⇒ setRewarItemCopy 实参
                // 写内联 begin 表达式（勿复用 item 引用；O0 无 CSE）。
                bf.m_bloodReward.setRewarItemCopy(
                    *result.m_dropItems.begin(), slot, i);
                // ORIG 0x085b8926 `CItemList::find_item(G_CDataManager()+0xc)`。
                CItem* card =
                    G_CDataManager()->m_itemList->find_item(item.m_addInfo);
                if (card != 0)
                    CLuckPoint::UseLuckPoint(m_member[slot].m_pUser,
                                             (int)(unsigned char)item.m_field1,
                                             card->get_rarity(),
                                             card->get_grade());
            }
        }
    }

    // ORIG 085b8a12 `movzbl 0x89f(%eax)` = CDungeon::m_bloodDungeonType
    //（unsigned char 宽度修正）
    if (flag && m_battleField.m_pDungeon->m_bloodDungeonType == 2)
    {
        int ultIdx = bf.m_bloodReward.getUltimateRewardItem();   // -0x24
        // ORIG 0x085b8a46 `CItemList::find_item(G_CDataManager()+0xc)`。
        CItem* card = G_CDataManager()->m_itemList->find_item(ultIdx);   // -0x20
        if (card != 0)
        {
            WongWork::stGenerateResult_t specific;
            Inven_Item made;                        // -0x14e（默认构造）
            // [R28wrt §34.2.2 字段写修复] ORIG 0x085b8a74/0x085b8a77
            //   `mov -0x20(%ebp),%eax; mov %eax,-0x148(%ebp)` = made(@-0x14a)+0x02
            //   = Inven_Item::m_addInfo（Inven_Item.h:269，unsigned int）← ultIdx
            //   （ORIG 槽 -0x20），位于 0x085b8aa2 `call create_item` **之前**
            //   （4.1 直落形：值先载 %eax 再 store，字段与 RHS 异型 ⇒ RHS-first）。
            //   消费链（缺写 ⇒ 读未初始化值）：create_item ORIG 0x08511fd2/0x08511ff3
            //   `mov 0x10(%ebp),%eax; mov 0x2(%eax),%eax` 直读 item+0x02 作
            //   find_item(int) 实参；找不到 ⇒ 落失败/错误日志块 ⇒ 邮件物品 idx 恒错。
            made.m_addInfo = ultIdx;
            G_CDataManager()->m_itemList->create_item(
                ENUM_ITEM_CREATE_TYPE_1, made, 1);
            specific.m_dropItems.push_back(made);
            const Inven_Item& item = *specific.m_dropItems.begin();
            bf.m_bloodReward.setRewarItemCopy(item, slot, round - 1);
        }
    }
}