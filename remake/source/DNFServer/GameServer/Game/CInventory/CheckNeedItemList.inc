bool CInventory::CheckNeedItemList(const std::vector<std::pair<int, int> >& items) const   // [b28d-Main 代落·S351 P292] int→bool
{
    // b28d-solo-CInventory [P212 全头部声明]：ORIG 0x08507b2b..0x08507b59 表明 8 个局部
    // （found/itemIdx/start/end/i/count/foundCnt/type=0xb）在循环前声明并初始化、
    // 循环内仅赋值；照此还原后 324/315 → 321/320（gap 9→1）。
    // 残余 1 = StartEndPos 实参装载序（P75 跨版本）+ 槽号，封 N82。
    // [J8InvenPvPWorld.CInventoryWriter 2026-09-11 · 本轮] ①`int type = 0xb` 改枚举
    // `Inven_Item::ITEM_TYPE type = Inven_Item::ITEM_TYPE_INVALID`（值同 0xb）⇒ StartEndPos
    // 实参装载序对齐 ORIG（0x08507c63..0x08507c7e 的 lea/%edx 组差异消失）；②type==5 分支
    // `if (m_pCreatureSlot[i].m_addInfo != itemIdx) continue; return 1;` 改
    // `if (... == itemIdx) return 1;`（P59 极性）⇒ our 324→323、差异块 12→9。
    // 残余 = 4.4/4.1 对「if (...) return/continue」的块序镜像（ORIG `je <return>`+fallthrough
    // i++；我方 `jne <i++>`+fallthrough return。ORIG `jl <return 0>`；我方 `jge <skip>`）
    // ⇒ N 类封顶（同源码两编译器镜像排布，无源码旋钮；已实测两种源码形均产 jne 形）。
    if (m_pParent->getGmQuestFlag())
    {
        return 1;
    }
    if (items.size() == 0)
    {
        return 1;
    }
    // ORIG（0x08507acc）：具名 it=begin() + pair 默认构造 + 循环头 found=false +
    // 临时 pair 右值赋值 + it++ 后置 ×4（三个前置判定分支各自推进 + item 处理共用尾）。
    typedef std::vector<std::pair<int, int> >::const_iterator Iterator;
    Iterator it = items.begin();
    std::pair<int, int> cur;
    // [b28d-solo] ORIG 0x08507b2b..0x08507b59：8 个局部在循环前声明并初始化
    // （found/itemIdx/start/end/i/count/foundCnt/type=0xb），循环内仅赋值（P212）。
    bool found = false;
    int itemIdx = 0;
    int start = 0;
    int end = 0;
    int i = 0;
    int count = 0;
    int foundCnt = 0;
    Inven_Item::ITEM_TYPE type = Inven_Item::ITEM_TYPE_INVALID;
    while (it != items.end())
    {
        found = false;
        cur = std::pair<int, int>(*it);
        itemIdx = cur.first;
        count = cur.second;
        if (itemIdx == 0)
        {
            if (m_money >= count)
            {
                it++;
            }
            else
            {
                return 0;
            }
        }
        else if (itemIdx == 2)
        {
            // ORIG 0x8507be8 call getPVPResultR → mov 0x58(%eax) = m_winPoint；
            // setge 物化形（正向 >= 判定，P222 正向包裹）。
            if (((CUserCharacInfo*)m_pParent)->getPVPResultR()->m_winPoint >= count)
            {
                it++;
            }
            else
            {
                return 0;
            }
        }
        else
        {
            type = GetItemType(itemIdx);
            if (type == 0)
            {
                it++;
            }
            else
            {
                StartEndPos((Inven_Item::ITEM_TYPE)type, start, end);
                if (type == 1)
                {
                    foundCnt = 0;
                    for (i = start; i <= end; i++)
                    {
                        if (m_pEquipSlot[i].m_addInfo != itemIdx)
                        {
                            continue;
                        }
                        foundCnt++;
                        if (foundCnt == count)
                        {
                            break;
                        }
                    }
                    if (foundCnt < count)
                    {
                        for (i = 3; i <= 8; i++)
                        {
                            if (m_pEquipSlot[i].m_addInfo == itemIdx)
                            {
                                foundCnt++;
                                if (foundCnt == count)
                                {
                                    break;
                                }
                            }
                        }
                    }
                    if (foundCnt < count)
                    {
                        for (i = 0; i <= 0x19; i++)
                        {
                            if (m_invenItem[i].m_addInfo == itemIdx)
                            {
                                foundCnt++;
                                if (foundCnt == count)
                                {
                                    break;
                                }
                            }
                        }
                    }
                    if (foundCnt < count)
                    {
                        return 0;
                    }
                }
                else if (type == 5)
                {
                    // ORIG 0x8507d8a：mov 0x55c(%eax) = m_invenItem[22].m_addInfo。
                    if (m_invenItem[22].m_addInfo == itemIdx)
                    {
                        return 1;
                    }
                    for (i = start; i <= end; i++)
                    {
                        if (m_pCreatureSlot[i].m_addInfo == itemIdx)
                        {
                            return 1;
                        }
                    }
                    return 0;
                }
                else
                {
                    for (i = start; i <= end; i++)
                    {
                        if (m_pEquipSlot[i].m_addInfo == itemIdx)
                        {
                            if (m_pEquipSlot[i].m_addInfo2 < count)
                            {
                                return 0;
                            }
                            found = true;
                            break;
                        }
                    }
                    if (!found)
                    {
                        for (i = 3; i <= 8; i++)
                        {
                            if (m_pEquipSlot[i].m_addInfo == itemIdx)
                            {
                                if (m_pEquipSlot[i].m_addInfo2 < count)
                                {
                                    return 0;
                                }
                                found = true;
                                break;
                            }
                        }
                        if (!found)
                        {
                            return 0;
                        }
                    }
                }
                it++;
            }
        }
    }
    return 1;
}