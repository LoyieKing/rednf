int CItemUpgrade::_ProcPenalty(CUser* user, Inven_Item* item,
                               const CItem* itemScript, unsigned short slot,
                               PacketGuard* packet)
{
    // [b28d-solo-CItemUpgrade] ORIG 0x08549ad4 无 amp 引用局部（P0）：
    //   `mov item,%eax; add $0x11,%eax; mov %eax,(%esp); call hasAbility` 逐次直取。
    const upgrade_table_t* table = item->m_amp.hasAbility()
        ? GlobalData::s_itemAmplifier_->getUpgradeInfoTable()
        : getUpgradeInfoTable();

    // ORIG 0x08549b0c 无 invenW 具名局部（P0）：getCurCharacInvenW() 的返回值
    // 直接作为 delete_item/insert_event_items 的 receiver（两次各自重调）。
    if (!user->getCurCharacInvenW()->delete_item(INVEN_TYPE_EQUIP, slot, 1,
                                                 (eItemDelReason)0xd, true))
        return 0x11;

    unsigned int upgrade = (unsigned int)(unsigned char)item->GetUpgrade();
    packet->put_short(slot);

    DisJointResult djr;
    // ORIG 0x08549b9e 无 dj/itemIdx 具名局部：itemIdx 跨 G_CDataManager() 保活于
    // %ebx，`mov 0x1c(%eax),%eax` 直取 m_disJoint 后立即传参。
    if (G_CDataManager()->m_disJoint->GetResult(item->m_addInfo, djr) != 0)
    {
        packet->put_byte(0);
        return 0;
    }

    item->m_addInfo = 0;
    std::vector<std::pair<int, int> > addItems;
    addItems.reserve(6);
    // [b28d-solo-CItemUpgrade] insertRet 声明前置于循环之前：ORIG 槽位序为
    // insertRet -0x20 / i -0x1c（4.4 块内按 lifetime 复用会把 i 排到 -0x20），
    // 按 P212「先声明深」排声明序复现。
    int insertRet;
    for (int i = 0; i < 5; ++i)
    {
        // 真 bug 修正（ORIG 0x08549c10 权威）：条件读 `mov -0x88(%ebp,%eax,4)`
        // （eax=i+4，djr@-0x8c ⇒ m_b[i]）。
        // [b28d-solo-CItemUpgrade] 实参顺序按 ORIG 0x08549c2d/0x08549c41 权威：
        // 先算 `&djr[i]`（m_a）→ %ecx → 0x8(%esp)（第二实参），再算
        // `&djr[i+0x14]`（m_b）→ %edx → 0x4(%esp)（第一实参）⇒ 源形是
        // `make_pair(m_b[i], m_a[i])`（原重建两字段顺序写反 ⇒ 真 bug）。
        // [b28d-solo-CItemUpgrade] ORIG 0x08549c1f `je <nop 岛>` + body 尾
        // `jmp 08549c6f` + `08549c6e: nop`（4.1 空 else 占位岛）⇒ P223 复现形
        // = 否定条件 goto 到体后空标签（正向 if 会被 4.4 jump-thread 掉 jmp+nop）。
        if (djr.m_b[i] == 0)
            goto push_done;
        addItems.push_back(std::make_pair(djr.m_b[i], djr.m_a[i]));
    push_done:
        ;
    }
    // [b28d-solo-CItemUpgrade] ORIG 0x08549ca0/0x08549ce4/0x08549d2b 三段：
    //   ①无条件 4 参版（局部 *table）；②if(hasAbility) 内 4 参版（**重算**
    //   getUpgradeInfoTable()）；③无条件 5 参版（重算 table + hasAbility 第 5 实参）。
    // 原重建只有「if→4 参 + 无条件 4 参」⇒ p167 MISSING hasAbility×1、
    // getUpgradeInfoTable×1、4 参版×1、5 参版×1。
    _getAdditionalDisjointItem(upgrade, itemScript, addItems, *table);
    if (item->m_amp.hasAbility())
        _getAdditionalDisjointItem(upgrade, itemScript, addItems,
                                   *getUpgradeInfoTable());
    _getAdditionalDisjointItem(upgrade, itemScript, addItems,
        *getUpgradeInfoTable(), item->m_amp.hasAbility());

    std::vector<int> outSlots;
    insertRet = user->getCurCharacInvenW()->insert_event_items(addItems,
        outSlots, (eMoneyAddReason)7, (eItemAddReason)0x13, false, "", "");
    packet->put_byte(insertRet);

    // [b28d-solo-CItemUpgrade] ORIG 0x08549dc5 循环：每次 `*it++` 三次取
    // slotVal/idx/cnt（无 it+=3、无 &0xffff 截断），随后**逐物品**补做
    // make_item + cube 统计 + 售价统计（原重建缺整段，属语义缺失，已补）。
    // [2026-09-10 判据⑮ 多余门禁] ORIG 0x08549d94 `call put_byte` 之后**直接**
    //   0x08549d99 `lea -0x3c; call begin`（无 insertRet 判空/判非零分支）⇒
    //   原码无 `if (insertRet != 0)` 包裹，删之照抄（outSlots 为空时循环自然不执行）。
    for (std::vector<int>::const_iterator it = outSlots.begin();
         it != outSlots.end(); )
    {
        int slotVal = *it++;
        int idx = *it++;
        int cnt = *it++;
        packet->put_short(slotVal);
        packet->put_item_idx((unsigned long)idx);
        packet->put_int(cnt);
        CItem* script = G_CDataManager()->find_item(idx);
        // [2026-09-10 P223] ORIG 0x08549e99 `je 08549f7d`（4.1 if-join 岛：
        //   false 路落 nop 占位）+ body 尾 `jmp 08549f7e`。正向 if 被 4.4
        //   jump-thread 掉 jmp+nop ⇒ 负条件 goto 到尾空标签复现（P223 定形）。
        if (script == 0)
            goto next_item;
        {
            Inven_Item invenItem;
            invenItem.m_addInfo = idx;
            invenItem.set_add_info(cnt);
            script->make_item(invenItem);
            GetInstanceCubeStatistic()->collectCubeStatistics(
                script, cnt, user, (CUBE_STATISTIC_FIELD)1);
            int sellPrice = 0;
            G_Store()->GetSellItemPrice(invenItem, script,
                (short)invenItem.m_addInfo2, false, sellPrice);
            GetInstanceValueStatistic()->AddValueStatistic(
                (VALUE_STATISTIC_FIELD)9, user, (unsigned int)sellPrice);
        }
    next_item:
        ;
    }
    return 0;
