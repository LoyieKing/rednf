int CInventory::delete_event_items(const std::vector<std::pair<int, int> >& items,
                                   std::vector<int>& outSlots, bool& bFound,
                                   eMoneySubReason moneyReason, eItemDelReason itemReason,
                                   bool bLog)
{
    // [SweepCInventory-2 复修] ORIG 675 / 我 674（gap 1，p167 CLEAN）。旧判词「ppEi 残差」被推翻，
    // 真结构差四处：①need 副本应为 amount 直接递减（ORIG +0x336 mov/sub/mov 三段）；
    // ②entry 声明在循环外（ORIG pair::C1Ev@+0x058）；③found 初始化在循环体首（ORIG +0x08e movb）；
    // ④start/end 无 =0 初始化（ORIG 直接传引用）。残余 1 = 局部与常量比较 4.4 恒折 mem 直比（N18b）。
    // [G8InvenPvP 2026-09-10 本轮 · ndiff 447→114，gap 归零 675=675，帧 0xc0→0xb0 对齐]
    // ① P227/P221：`cMyTrace trace(...); trace(...)` → 全表达式临时链
    //   `cMyTrace(...)(...)`（ORIG 0x0850? 该 trace 为构造即调用）⇒ 互斥块对象槽翻正。
    // ② P160/P162③ 槽复用：ORIG **全函数只有一个循环下标槽 -0x68**（5 个 for 与
    //   两处 `movl $0x3,-0x68(%ebp)` 重置共用），旧源码每处 `for (int i = ...)`
    //   各占独立槽（-0x80/-0x84/-0x8c/-0x90）⇒ 帧 0xc0 vs ORIG 0xb0、我方槽 34 vs 31。
    //   改块级单变量 `int i;` + 循环内赋值式后：帧 0xb0、槽 30、指令 675=675。
    // 残余 = 具名标量槽号分区（ORIG deleteCount@-0xc/itemIdx@-0x14/found@-0x15
    // vs 我方 -0x18/-0x10/-0x11）⇒ 4.4 按首次使用分配（N98/N82附）封顶。
    if (m_pParent->getGmQuestFlag())
    {
        return 1;
    }
    // [G8InvenPvP 2026-09-10 · 槽序封顶] 残余槽号差（ORIG deleteCount@-0xc/
    // itemIdx@-0x14/found@-0x15 vs 我方 -0x18/-0x10/-0x11）属 4.4「按首次使用
    // 顺序分配 + 聚合对象优先占低槽」的分区策略（N98/N82附）；ORIG 的
    // `movl $0x0,-0xc(%ebp)` 位置与我方逐条相同（指令序已对齐），移声明只会
    // 改指令序 ⇒ 维持，判 N。
    int deleteCount = 0;
    if (items.size() == 0)
    {
        return deleteCount;   // P0：ORIG 0x08505dfe `mov -0xc(%ebp),%eax`（返回具名局部，非字面量 0）
    }
    // P50 先声明后赋值 + P135 具名 pair 右值赋值保持。
    std::pair<int, int> entry;
    std::vector<std::pair<int, int> >::const_iterator it;
    it = items.begin();
    while (it != items.end())
    {
        bool found = false;
        entry = std::pair<int, int>(*it);
        int itemIdx = entry.first;
        int amount = entry.second;
        if (itemIdx == 0)
        {
            if (use_money(amount, moneyReason, true))
            {
                deleteCount++;
                outSlots.push_back(0);
                outSlots.push_back(0);
                outSlots.push_back(amount);
            }
            if (moneyReason == (eMoneySubReason)4 && m_pParent != 0)
            {
                GetInstanceValueStatistic()->AddValueStatistic(
                    (VALUE_STATISTIC_FIELD)0x11, m_pParent, (unsigned int)amount);
            }
            it++;
            continue;
        }
        else if (itemIdx == 2)
        {
            // [P41] GetWinPoint 已 unsigned（SweepCPrivateStore 头改动）⇒ 删冗余 cast，
            // 试复现 ORIG 0x08505f5b `cmp %edx,%eax; setae %al`。
            if (((CUserCharacInfo*)m_pParent)->GetWinPoint() >= (unsigned int)amount)
            {
                m_pParent->UseWinPoint(amount, (eWPSubReason)3);
                deleteCount++;
                outSlots.push_back(0);
                outSlots.push_back(2);
                outSlots.push_back(amount);
            }
            it++;
            continue;
        }
        else
        {
            int type = GetItemType(itemIdx);
            if (type == 0)
            {
                continue;
            }
            int start;
            int end;
            // [G8InvenPvP 2026-09-10 · P160/P162③ 复用 ORIG 复用的槽] ORIG 全函数
            // 只用**一个**循环下标槽 -0x68（5 个 for 与两处 `i=3` 重置共用，
            // 0x0850?..直证 `movl $0x3,-0x68(%ebp)`/`add $0x1,-0x68`），我方
            // 每个 `for (int i = ...)` 各占独立槽（-0x80/-0x84/-0x8c/-0x90）⇒
            // 帧 0xc0 vs ORIG 0xb0。按 P0/P160③ 提为块级单变量，循环内改用赋值式。
            int i;
            StartEndPos((Inven_Item::ITEM_TYPE)type, start, end);
            if (type == 1)
            {
                for (i = start; i <= end; i++)
                {
                    if (m_pEquipSlot[i].m_addInfo == itemIdx)
                    {
                        deleteCount++;
                        outSlots.push_back(0);
                        outSlots.push_back(i);
                        outSlots.push_back(m_pEquipSlot[i].m_addInfo2);
                        delete_item(INVEN_TYPE_EQUIP, i, 1, itemReason, true);
                        amount = amount - 1;
                        if (amount == 0)
                        {
                            break;
                        }
                    }
                }
                if (amount > 0)
                {
                    for (i = 3; i < 9; i++)
                    {
                        if (m_pEquipSlot[i].m_addInfo == itemIdx)
                        {
                            deleteCount++;
                            outSlots.push_back(0);
                            outSlots.push_back(i);
                            outSlots.push_back(m_pEquipSlot[i].m_addInfo2);
                            delete_item(INVEN_TYPE_EQUIP, i, 1, itemReason, true);
                            amount = amount - 1;
                            if (amount == 0)
                            {
                                break;
                            }
                        }
                    }
                }
                if (amount > 0)
                {
                    for (i = 0; i < 0x1a; i++)
                    {
                        if (m_invenItem[i].m_addInfo == itemIdx)
                        {
                            if (i < 0xc)
                            {
                                bFound = true;
                            }
                            deleteCount++;
                            outSlots.push_back(3);
                            outSlots.push_back(i);
                            outSlots.push_back(m_invenItem[i].m_addInfo2);
                            delete_item(INVEN_TYPE_INVEN, i, 1, itemReason, true);
                            amount = amount - 1;
                            if (amount == 0)
                            {
                                break;
                            }
                        }
                    }
                }
                if (amount == 0)
                {
                    found = true;
                }
            }
            else
            {
                for (i = 3; i < 9; i++)
                {
                    if (m_pEquipSlot[i].m_addInfo == itemIdx &&
                        amount <= m_pEquipSlot[i].m_addInfo2)
                    {
                        delete_item(INVEN_TYPE_EQUIP, i, amount, itemReason, true);
                        found = true;
                        deleteCount++;
                        outSlots.push_back(0);
                        outSlots.push_back(i);
                        outSlots.push_back(amount);
                        if (m_pParent != 0 && (int)itemReason == 8 && bLog)
                        {
                            GetInstanceCubeStatistic()->collectCubeStatistics(
                                itemIdx, amount, m_pParent,
                                (CUBE_STATISTIC_FIELD)0x65);
                        }
                        else if (m_pParent != 0 && (int)itemReason == 8 && !bLog)
                        {
                            GetInstanceCubeStatistic()->collectCubeStatistics(
                                itemIdx, amount, m_pParent,
                                (CUBE_STATISTIC_FIELD)0x64);
                        }
                        else if (m_pParent != 0 && (int)itemReason == 4)
                        {
                            GetInstanceCubeStatistic()->collectCubeStatistics(
                                itemIdx, amount, m_pParent,
                                (CUBE_STATISTIC_FIELD)0x66);
                        }
                        break;
                    }
                }
                if (!found)
                {
                    for (i = start; i <= end; i++)
                    {
                        if (m_pEquipSlot[i].m_addInfo == itemIdx &&
                            amount <= m_pEquipSlot[i].m_addInfo2)
                        {
                            delete_item(INVEN_TYPE_EQUIP, i, amount, itemReason, true);
                            found = true;
                            deleteCount++;
                            outSlots.push_back(0);
                            outSlots.push_back(i);
                            outSlots.push_back(amount);
                            if (m_pParent != 0 && (int)itemReason == 8 && bLog)
                            {
                                GetInstanceCubeStatistic()->collectCubeStatistics(
                                    itemIdx, amount, m_pParent,
                                    (CUBE_STATISTIC_FIELD)0x65);
                            }
                            else if (m_pParent != 0 && (int)itemReason == 8 && !bLog)
                            {
                                GetInstanceCubeStatistic()->collectCubeStatistics(
                                    itemIdx, amount, m_pParent,
                                    (CUBE_STATISTIC_FIELD)0x64);
                            }
                            else if (m_pParent != 0 && (int)itemReason == 4)
                            {
                                GetInstanceCubeStatistic()->collectCubeStatistics(
                                    itemIdx, amount, m_pParent,
                                    (CUBE_STATISTIC_FIELD)0x66);
                            }
                            break;
                        }
                    }
                }
            }
            if (!found)
            {
                // [G8InvenPvP 2026-09-10 · P227/P221] 试「构造即调用」全表达式
                // 临时链（SendAvatarEvent 实证可翻互斥块对象槽方向）。
                cMyTrace(__PRETTY_FUNCTION__, 0x1d3a, 5)(
                    "NOT CRITICAL BUT FOR OBSERVING, No Event Item %d FOUND!", itemIdx);
                return 0;
            }
        }
        it++;
    }
    if (deleteCount != (int)items.size())
    {
        // [ORIG 0x0850? 直证] ORIG 该臂为 `je T40; mov $0x0,%eax; jmp <end>; T40:
        // mov -0xc(%ebp),%eax`（返回常量 0，**非**先写槽再读回）⇒ 改早返回形。
        return 0;
    }
    return deleteCount;
