bool CDeathTower::_makeDropItem(CUser* user, char type, int slot, int count, map_item& item)
{
    // [b28d-solo-CDeathTower2 本轮读数] orig 489 / our 494（gap −3，前值 510）：
    // ①【真 bug 修正】字段填充段首行原写 `item.m_item.m_field0 = 0`（+0x10），
    //   ORIG 0x0846676x `movb $0x0,(%eax)` 实为 map_item+0x00 = m_count
    //   ⇒ m_count 此前从未清零（掉落堆叠计数未初始化）；已改 `item.m_count=false`。
    // ②【P0/P48 槽往返 ×9】case 0/2/3/7 的 getCurCharacInvenR/CargoR/W 结果与
    //   getCurCharacInvenW（use_money/delete_item）全部改链式内联（ORIG %eax 直传
    //   this，0x0846642d/44e0/4695c/469d0 实证）⇒ 消 18 条 store/load 往返。
    // ③【残留 −3，判 N】switch(GetAttachType) ORIG 0x0846676c 为 6 项跳表
    //   `cmp $5,%eax; ja; jmp *table`（3 条），4.4 对同形恒发决策树（5 条 cmp/jcc，
    //   补 case 0/4 实测 505 更劣）⇒ 4.1↔4.4 switch lowering 代际差（P320 族）；
    //   另 m_amp 结构赋值写序（ORIG +0x15/+0x19/+0x1d 后写 +0x11）+ isEquipableItemType
    //   调用点 ORIG `xor $1`（负极性 if，P222）均为形态残差。
    // [B24D-CDeathTower 语句序还原] 字段填充段按 ORIG 0x084667d8..0x0846688a 重排：
    //   m_fieldb(+0x1d) 提到 m_amp 之前；m_amp 拆为 guid 视图(+0x25/+0x29/+0x2d，
    //   单条结构赋值) 后接 4B 头视图(+0x21，独立重载)；m_fieldd(+0x1b)/m_field0(+0x10)
    //   移到 SetItemAttr 之后（ORIG 0x08466878/0x08466883）。见该段行内注释。
    // P167：41/41 CLEAR（Inven_Item C1/C2 同址别名 P93 伪差）。
    // ORIG T 0x0846638c（Ghidra 反编译语义逐条转写；参数槽：0x8=this,0xc=user,
    // 0x10=type,0x14=slot,0x18=count,0x1c=item&）
    Inven_Item invenItem;   // ORIG 调 _ZN10Inven_ItemC1Ev（0x80cb854）

    switch (type)
    {
    case 0: // money or equip（ORIG 判断 slot==0 为金钱）
        if (slot == 0)
        {
            // money item（ORIG +0x4d：m_count=0 / m_field1=2 / m_addInfo=0 /
            // set_add_info(count) / m_fieldb=0 / ResetItemAttr）
            item.m_count = 0;
            item.m_item.m_field1 = 2;
            item.m_item.m_addInfo = 0;
            item.m_item.set_add_info(count);
            item.m_item.m_fieldb = 0;
            item.m_item.ResetItemAttr();
        }
        else
        {
            // [b28d-solo-CDeathTower2 P0] ORIG 0x0846642d..44b：getCurCharacInvenR
            // 返回值在 %eax 直传 GetInvenSlot this（无具名槽往返）⇒ 链式。
            invenItem = user->getCurCharacInvenR()->GetInvenSlot(1, slot);
        }
        break;
    case 2: // cargo
    {
        // [b28d-solo-CDeathTower2 P0] ORIG 0x084664e0..4f2：%eax 直传。
        invenItem = user->getCurCharacCargoR()->get_cargo_slot(slot);
        break;
    }
    case 3: // avatar（ORIG GetInvenSlot space=0）
    {
        invenItem = user->getCurCharacInvenR()->GetInvenSlot(0, slot);
        break;
    }
    case 7: // creature（ORIG GetInvenSlot space=3）
    {
        invenItem = user->getCurCharacInvenR()->GetInvenSlot(3, slot);
        if (invenItem.m_field1 == 5)          // ITEM_TYPE_CREATURE_EQUIP → 拒绝
        {
            return false;
        }
        break;
    }
    default:
        return false;
    }

    // 公共校验 + 字段填充（ORIG +0x377 起）
    int sellPrice = 0;
    if (invenItem.m_addInfo != 0)             // +0x37e：itemId 非 0 才校验
    {
        CItem* citem = G_CDataManager()->find_item(invenItem.m_addInfo);
        if (!citem)
        {
            return false;
        }
        int attachType = citem->GetAttachType();
        switch (attachType)
        {
        case 1:
        case 2:
        case 5:
            return false;
        case 3:
            if (invenItem.m_field0 != 1)
            {
                return false;
            }
            break;
        }
        if (invenItem.m_upgradeSep.IsTradeRestriction())
        {
            return false;
        }
        if (invenItem.isEquipableItemType())
        {
            // ORIG 0x84666fa：CEquipItem+0x234 = m_itemType（见 CBossStage.cpp 同款）
            const CEquipItem* equip = static_cast<const CEquipItem*>(citem);
            if (equip->m_itemType == 0xb || equip->m_itemType < 10)
            {
                return false;
            }
        }
        // 字段填充（ORIG +0x44c）
        // [b28d-solo-CDeathTower2 真 bug 修正] ORIG 0x0846676x `mov 0x1c(%ebp),%eax;
        // movb $0x0,(%eax)` 写 map_item+0x00 = m_count（非 m_item+0x00）；
        // 旧实现误写 item.m_item.m_field0（+0x10）致 m_count 未清零，且 +0x10
        // 随后被 invenItem.m_field0 覆盖 ⇒ 净多 1 条指令。
        item.m_count = false;
        item.m_item.m_field1 = invenItem.m_field1;
        item.m_item.m_addInfo = invenItem.m_addInfo;
        item.m_item.m_fieldb = invenItem.m_fieldb;
        // [B24D-CDeathTower 语句序还原] ORIG 0x084667d8..0x0846688a 实测写序：
        //   +0x00(m_count) → +0x11(m_field1) → +0x12(m_addInfo) → +0x1d(m_fieldb)
        //   → guid 组 +0x25/+0x29/+0x2d（单条结构赋值，共享一次基址装载）
        //   → +0x21(m_amp0Head，独立重载，0x08466811 `mov 0x1c(%ebp),%eax;
        //     mov -0x44(%ebp),%edx; mov %edx,0x21(%eax)`）
        //   → isEquipable 检查/set_add_info/SetItemAttr
        //   → +0x1b(m_fieldd, 0x08466878) → +0x10(m_field0, 0x08466883)。
        // 原实现把 m_amp 当整体 14B 复制（4 写共享一次基址装载 ⇒ 少一次 0x1c
        // 重载）、并把 fieldd/field0 提前到 SetItemAttr 之前 ⇒ 两个 hunk 错位。
        // 按 P247 拆 guid 视图 + 4B 头部视图（与 ORIG 逐条对齐）。
        item.m_item.m_amp.m_guid = invenItem.m_amp.m_guid;
        item.m_item.m_amp.m_amp0Head = invenItem.m_amp.m_amp0Head;
        if (invenItem.isEquipableItemType())
        {
            item.m_item.set_add_info(count);
        }
        else
        {
            item.m_item.set_add_info(invenItem.m_addInfo2);
        }
        item.m_item.SetItemAttr(invenItem.GetItemAttr());
        item.m_item.m_fieldd = invenItem.m_fieldd;
        item.m_item.m_field0 = invenItem.m_field0;
        G_Store()->GetSellItemPrice(invenItem, citem, (short)count, false, sellPrice);
    }

    // 删除/扣费 + 统计（ORIG +0x535 起）
    switch (type)
    {
    case 0:
        if (slot == 0)
        {
            // [b28d-solo-CDeathTower2 P0/P48] ORIG 0x084668f2..91e：getCurCharacInvenW
            // 返回值 %eax 直落实参槽（无 invenW 具名槽）。
            if (user->getCurCharacInvenW()->use_money(count, (eMoneySubReason)6, true) != 1)
            {
                return false;
            }
            GetInstanceValueStatistic()->AddValueStatistic(
                (VALUE_STATISTIC_FIELD)0x17, user, (unsigned int)count);
        }
        else
        {
            // [b28d-solo-CDeathTower2 P0/P48] ORIG 0x0846695c..990：%eax 直传 this。
            if (user->getCurCharacInvenW()->delete_item((INVEN_TYPE)1, slot, count,
                                    (eItemDelReason)6, true) != 1)
            {
                return false;
            }
            GetInstanceValueStatistic()->AddValueStatistic(
                (VALUE_STATISTIC_FIELD)0x16, user, (unsigned int)sellPrice);
        }
        break;
    case 2:
    {
        // [b28d-solo-CDeathTower2 P0/P48] ORIG 0x084669d0..9f1：%eax 直传 this。
        if (user->getCurCharacCargoW()->delete_item(slot, count, (eItemDelReason)0x10) != 1)
        {
            return false;
        }
        break;
    }
    case 3:
    {
        if (user->getCurCharacInvenW()->delete_item((INVEN_TYPE)0, slot, 1,
                                (eItemDelReason)6, true) != 1)
        {
            return false;
        }
        GetInstanceValueStatistic()->AddValueStatistic(
            (VALUE_STATISTIC_FIELD)0x16, user, (unsigned int)sellPrice);
        break;
    }
    case 7:
    {
        if (user->getCurCharacInvenW()->delete_item((INVEN_TYPE)3, slot, 1,
                                (eItemDelReason)6, true) != 1)
        {
            return false;
        }
        GetInstanceValueStatistic()->AddValueStatistic(
            (VALUE_STATISTIC_FIELD)0x16, user, (unsigned int)sellPrice);
        break;
    }
    default:
        return false;
    }
    return true;
