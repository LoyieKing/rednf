int CInventory::CheckEquipmentItemCondition(
    const std::vector<std::pair<int, int> >& items, stEquipCondition& cond,
    Inven_Item& outItem) const
{
    // ORIG 0x8507f21-0x8507f33：size()==0 的 sete 物化形（P197④；empty() 无该段）。
    if (items.size() == 0)
        return 0;

    // ORIG 局部集合/初值/槽序（0x8507f3f-0x8507f94 逐条对照）：
    //   it@-0x50(C1Ev begin)、itemInfo@-0x58(C1Ev)、reserved@-0x31(movb 0，函数头+
    //   每轮循环体各清零一次的死局部 P171)、itemIdx@-0x30、start@-0x5c、end@-0x60、
    //   slot@-0x2c、count@-0x28、found@-0x24、type@-0x20(初值 0xb=ITEM_TYPE_INVALID)。
    // 循环为 4.1 的「底部条件」布局（P283）：体在前、条件块在函数后部，用 goto 尾标签复现。
    std::vector<std::pair<int, int> >::const_iterator it = items.begin();
    std::pair<int, int> itemInfo;
    char reserved = 0;
    int itemIdx = 0;
    int start = 0;
    int end = 0;
    int slot = 0;
    int count = 0;
    int found = 0;
    Inven_Item::ITEM_TYPE type = Inven_Item::ITEM_TYPE_INVALID;
    for (; it != items.end(); )
    {
        reserved = 0;
        // ORIG 0x8507fa8-0x8507fc0：operator* → 8 字节临时 → pair::operator=(pair&&)
        // （aSEOS0_，P135 右值赋值形）。
        itemInfo = std::pair<int, int>(*it);
        itemIdx = itemInfo.first;
        count = itemInfo.second;
        type = static_cast<Inven_Item::ITEM_TYPE>(GetItemType(itemIdx));
        if (type != Inven_Item::ITEM_TYPE_EQUIP)
        {
            it++;                          // ORIG 0x8508001 ppEi（P28 后置形）
            continue;
        }
        // ORIG 0x850800e-0x8508031：type==0 的死分支（P184 死块照抄；type==1 时不可达）。
        if (type == Inven_Item::ITEM_TYPE_0)
        {
            it++;
            continue;
        }
        if (count > 1)
            return 1;
        StartEndPos(type, start, end);
        found = 0;
        slot = start;
        goto L_cond1;
        do
        {
            if (m_pEquipSlot[slot].m_addInfo == itemIdx)
            {
                found++;
                if (found > 1)
                    return 0x12;
                outItem = m_pEquipSlot[slot];
                // ORIG 0x850812b/0x8508133：CheckItemLock(1, slot) —— 实参序 (1, slot)，
                // 旧码 (slot,1) 为真语义 bug（与 AvatarRechargeServer 等调用点惯例一致）。
                if (m_pParent->CheckItemLock(1, slot))
                    return 0xd5;
            }
            slot++;
        L_cond1: ;
        } while (slot <= end);
        slot = 3;
        goto L_cond2;
        do
        {
            if (m_pEquipSlot[slot].m_addInfo == itemIdx)
            {
                found++;
                if (found > 1)
                    return 0x12;
                outItem = m_pEquipSlot[slot];
                if (m_pParent->CheckItemLock(1, slot))
                    return 0xd5;
            }
            slot++;
        L_cond2: ;
        } while (slot <= 8);
        it++;
    }

    // ORIG 0x850829a-0x85082a2：条件为 0 时 je 到函数尾部共享的 return-0 块（0x85083c6），
    // 主流程末尾 fall-through 到同一块（P214②/P222 正向包裹共享尾块）。
    if (outItem.m_addInfo != 0)
    {
        // ORIG 0x85082a8-0x8508300：find_item 结果直接落 CEquipItem* 局部(-0x1c)。
        CEquipItem* equip =
            static_cast<CEquipItem*>(G_CDataManager()->find_item(outItem.m_addInfo));
        if (equip == 0)
            return 0x11;

        // ORIG 0x85082e2-0x85082f0 `cmpb $0x80`：局部必须是 unsigned char（char 下
        // 4.4 对 `==0x80` 常量折叠删整段分支，实测丢 cmpb/je/mov$0x13 三条）。
        const unsigned char abilityType = outItem.m_amp.getAbilityType();
        if (abilityType == 0x80)
            return 0x13;

        // ORIG 0x85082f5-0x8508317：get_endurance 先落槽(-0x14)，再读 m_fieldb 内存直比。
        int endurance = equip->get_endurance();
        if (static_cast<int>(outItem.m_fieldb) != endurance)
            return 0xd8;

        cond.m_upgrade = 1;
        cond.m_tableIdx = outItem.GetUpgrade();
        if (abilityType == 0)
        {
            cond.m_amplifySelect = 0;
            cond.m_pad3 = 0;
            cond.m_field4 = 0;
        }
        else
        {
            cond.m_amplifySelect = 1;
            // 与 m_pad3(uchar) 同型 ⇒ LHS-first 装载序（P211/P248 档③）。
            cond.m_pad3 = abilityType;
            cond.m_field4 = outItem.m_amp.getAbilityValue();
        }

        // ORIG 0x8508378-0x85083b1：CItem* 局部(-0x10)=equip 指针拷贝；GetAttachType
        // 结果落槽(-0xc) 后内存直比；m_flag6 三分支独立 movb（非三目）。
        CItem* item = equip;
        int attachType = item->GetAttachType();
        if (attachType == 0)
            cond.m_flag6 = 1;
        else if (outItem.m_field0)
            cond.m_flag6 = 1;
        else
            cond.m_flag6 = 0;

        // cond+0x07 ← GetReSealCount()（stEquipCondition::m_pad7 承载 reseal 计数）
        cond.m_pad7 = outItem.GetReSealCount();
    }
    return 0;
