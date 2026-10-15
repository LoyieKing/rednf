// ============================================================================
// _CheckCondition @ 0x8546342
//   校验强化/增幅条件，返回错误码（0=通过）。
// [2026-09-10 判词] DIFF（gap=2、p167 CALLSITE CLEAR 32/32）。残余 = 2 处
//   `m_i32[rarity+0x104]`：ORIG 0x08546603/0x08546635 `add $0x104,%edx;
//   mov 0xc(%eax,%edx,4)` vs 4.4 `lea 0x104(%eax),%edx`（P30 结构性伪影；
//   已实测 P311 二维指针视角反劣 ourN 323→329 ⇒ 回滚）⇒ N4 封顶。
//   本轮修 P0 内联/P222 正向包裹/UPG_TBL_I32 视图/predict 两句式/upType if-else。
//   本轮已修：删 repeatEvent 判空（ORIG 0x0854639f 直接解引用）、内联 reduce
//   （无具名局部）。残余 = 4.4 对具名局部的槽化 vs ORIG 的 %ebx/%esi 寄存器
//   保活（N82/N95 帧差 0xa4 vs 0x84）+ 常量折叠形差 ⇒ N 封顶。
// [L6S6 2026-09-11 复验] 2 处 `add $0x104,%edx; mov 0xc(%eax,%edx,4)` 的探针穷举：
//   /tmp/L6S6probe/p1.cpp g1-g4（int/enum 返回、加法交换、unsigned 局部）4.4.7
//   恒产 `lea 0x104(%eax),%edx` ⇒ ORIG 为 4.1 独有段，4.4 无源码解 ⇒ N4 确认封顶。
// ============================================================================
// [round3-L29 2026-09-10 复核] 判据⑪ MIXED（OURS-only=[-101,-99,-40,-36] vs
//   ORIG-only=[-116,-112,-93,-91]）⇒ 相对序差；观测 -0x20/-0x10(ORIG) vs -0x28/-0x18(我方)。
//   本轮未及逐对重排（轮次预算）；grep 本注释可避免重复排查方向。
int CItemUpgrade::_CheckCondition(const CUser* user, const Inven_Item& item,
                                  const CItem* itemScript,
                                  const Inven_Item& ticket,
                                  const upgrade_table_t& table,
                                  const upgrade_info_t& info)
{
    if (!item.m_amp.isIdentified())
        return 0xae;
    if (ticket.m_addInfo != info.m_recipeItem)
        return 0x16;
    if (ticket.m_addInfo2 < info.m_meterialCount)
        return 0x16;

    // ORIG 0x085463d4：GetRepeatEvent(0x32) 调两次（IsEventing 判活 + GetReduceRate）。
    int discountGroup = 0;
    // [b28d-solo-CItemUpgrade] ORIG 0x0854639f **无** repeatEvent 具名局部、也
    // **无判空**（P171/P140）：`call GetRepeatEvent; mov (%eax),%edx;
    // add $0x34,%edx; mov (%edx),%edx; ...; call *%edx` 直接走 vtable+0x34
    // （slot 13 = IsEventing(CUser*)）。第二次调用内联进 GetReduceRate 的 receiver
    // （ORIG `mov %eax,(%esp); call GetReduceRate`，无 reduce 局部槽）。
    if (GlobalData::s_event_manager->GetRepeatEvent(0x32)->IsEventing(0) &&
        !item.m_amp.hasAbility())
    {
        int rate = ((CReduceUpgradeItemPay*)
            GlobalData::s_event_manager->GetRepeatEvent(0x32))->GetReduceRate();
        if (0 < rate)
        {
            discountGroup = info.m_discountBase -
                            (info.m_discountBase * rate) / 100;
            if (discountGroup < 0)
                discountGroup = 0;
        }
    }
    else
    {
        discountGroup = info.m_discountBase;
    }

    // ORIG 0x08546468 无 inven 具名局部（P0）：getCurCharacInvenR() 结果直接
    // 作 GetInvenSlot 的 receiver（`mov %eax,0x4(%esp)`）。
    Inven_Item slotItem = user->getCurCharacInvenR()->GetInvenSlot(0, 0xb);
    int slotItemIdx = slotItem.m_addInfo;
    // ORIG 0x0854649b 无 dm 具名局部：G_CDataManager() 结果直接作 find_item 的
    // receiver。
    CEquipItem* materialScript =
        static_cast<CEquipItem*>(G_CDataManager()->find_item(slotItemIdx));
    if (materialScript != 0)
    {
        // ORIG：折扣 = ROUND(getUpgradeCostDiscount(material) * discountGroup /
        //                     _DAT_08c9d304 + discountGroup)。_DAT_08c9d304 为该
        // 强化成本基准浮点常量（== -100.0f）。ORIG 0x085464ca 返回值直接 fmuls
        // （无 fstps 槽）⇒ 无具名 float 局部（P0）；两次 `fildl -0x20` 即两次
        // (float)discountGroup（P243 书写序 out = out + in）。
        // ORIG 0x085464b8..0x085464c1 两次 `fildl -0x20; fstps` 到两个不同槽
        // （-0x74/-0x70）⇒ 两个具名 float 局部（同值）；末项 fadds -0x74 在内存
        // （P243 书写序）。
        float baseA = (float)discountGroup;
        float baseB = (float)discountGroup;
        discountGroup =
            (int)(materialScript->getUpgradeCostDiscount() * baseB / -100.0f +
                  baseA);
    }

    // 货币检查（ORIG 429 行重新 getCurCharacInvenR，不复用 inven）。
    if (user->getCurCharacInvenR()->get_money() < discountGroup)
        return 10;

    if (table.m_infos.size() <= item.GetUpgrade())   // ORIG 436 行 GetUpgrade
        return 0xd;
    if (item.m_itemType != 1)
        return 0x13;

    // [b28d-solo-CItemUpgrade] ORIG 顺序（0x0854655d 先范围、0x085465a5 后 ==0xb）：
    // 先 `type <= 9 || type > 0x15`，再 `type == 0xb`（原重建写反）；且 P48 无 CSE，
    // 每次比较各自独立调用 GetItemType（无 itemType 具名局部）。
    if (itemScript->GetItemType() <= 9 || itemScript->GetItemType() > 0x15)
        return 0x13;
    if (itemScript->GetItemType() == 0xb)
        return 0x13;

    // ORIG 0x085465cb 无 endurance 具名局部（P0）：`movzwl 0xb(%eax),%eax;
    // movzwl %ax,%ebx` 直用字段（P77）。
    if (item.m_endurance != ((const CEquipItem*)itemScript)->get_endurance())
        return 7;

    // ORIG 0x8546600：读 table+(rarity+0x104)*4+0xc（intAtConst 减 0xc 内偏）；
    // 两步寻址形 `add $0x104,%edx; mov 0xc(%eax,%edx,4)` 需数组下标视图
    // （UPG_TBL_I32，P30/P309；指针算术版恒折 lea/shl）。
    // [2026-09-10 N4 实测] 尝试 P311 二维指针视角
    //   `((int(*)[0x104])(&m_i32[0]))[1][rarity]` ⇒ 4.4 改产
    //   `mov 0x41c(%eax,%edx,4)` +6 条（ourN 323→329，反劣）⇒ 回滚为
    //   一维 `m_i32[rarity+0x104]` 的 `lea 0x104(%eax),%edx` 形（P30 结构性伪影，
    //   与 ORIG 的 `add $0x104,%edx` 差 1 条/处）。
    if (UPG_TBL_I32(&table, itemScript->get_rarity() + 0x104) != -1)
    {
        // ORIG 465 行内层比较再调一次 get_rarity（不复用外层索引）；
        // ORIG 形 `cmp %eax,%ebx; setge` ⇒ 源写 upgrade >= penTable。
        if ((int)item.GetUpgrade() >=
            UPG_TBL_I32(&table, itemScript->get_rarity() + 0x104))
            return 0x5f;
    }

    // ORIG 0x0854665d 无 rarity/penalty 具名局部：get_rarity 结果在 %ebx 直传，
    // GetPenaltyType 结果直接 `cmp $0x3; sete; test; je <check_level>` 物化
    // （P0 + P222 正向包裹：penalty==3 才进 predict 块）。
    if (table.GetPenaltyType(item.GetUpgrade(),
                             (ENUM_RARITY)itemScript->get_rarity(),
                             &info) == 3)
    {
        // 若 penalty 类型==3，预测结果物品数检查（InvenEmptycount+QuickEmptyCount）。
        // ORIG 0x8546695：predict = getPredictResultItemTypeCount(rarity)+1，hasAbility 再 +1；
        // predict==0 跳过检查；<predict 返回 0x15。
        // ORIG 0x0854670c `cmp -0xc(%ebp),%eax; jae`（mem 直比 + 无符号序）⇒
        // predict 是 unsigned 槽（P209）；ORIG 0x085466b6/0x085466b9 为
        // `mov %eax,槽; addl $0x1,槽` 两句式（单表达式形恒折 add+store）。
        unsigned int predict =
            (unsigned int)G_CDataManager()->m_disJoint
                ->getPredictResultItemTypeCount(itemScript->get_rarity());
        ++predict;
        if (item.m_amp.hasAbility())
            predict += 1;
        if (predict != 0 &&
            user->getCurCharacInvenR()->InvenEmptycount(Inven_Item::ITEM_TYPE_3) +
                    user->getCurCharacInvenR()->QuickEmptyCount() <
                predict)
            return 0x15;
    }
check_level:
    // ORIG 0x08546728 `movl $0x0,槽` 预置 + if/else 直接 store（三元形产 eax 中转）。
    int upType = 0;
    if (item.m_amp.hasAbility())
        upType = 2;
    else
        upType = 1;
    if (((CEquipItem*)itemScript)->isUpgradableLevel(
            static_cast<ItemUpgradeType::T>(upType),
            user->get_charac_level()) == false)
        return 0x13;
    return 0;
}