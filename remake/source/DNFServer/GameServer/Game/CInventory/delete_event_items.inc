int CInventory::delete_event_items(const std::vector<std::pair<int, int> >& items,
                                   std::vector<int>& outSlots, bool& bFound,
                                   eMoneySubReason moneyReason, eItemDelReason itemReason,
                                   bool bLog)
{
    // [SweepCInventory-2 复修] ORIG 675 / 我 674（gap 1，p167 CLEAN）。旧判词「ppEi 残差」被推翻，
    // 真结构差四处：①need 副本应为 amount 直接递减（ORIG +0x336 mov/sub/mov 三段）；
    // ②entry 声明在循环外（ORIG pair::C1Ev@+0x058）；③found 初始化在循环体首（ORIG +0x08e movb）；
    // ④start/end 无 =0 初始化（ORIG 直接传引用）。残余 1 = 局部与常量比较 4.4 恒折 mem 直比（N18b）。
    if (m_pParent->getGmQuestFlag())
    {
        return 1;
    }
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
            StartEndPos((Inven_Item::ITEM_TYPE)type, start, end);
            if (type == 1)
            {
                for (int i = start; i <= end; i++)
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
                    for (int i = 3; i < 9; i++)
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
                    for (int i = 0; i < 0x1a; i++)
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
                for (int i = 3; i < 9; i++)
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
                    for (int i = start; i <= end; i++)
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
                cMyTrace trace(__PRETTY_FUNCTION__, 0x1d3a, 5);
                trace("NOT CRITICAL BUT FOR OBSERVING, No Event Item %d FOUND!", itemIdx);
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
