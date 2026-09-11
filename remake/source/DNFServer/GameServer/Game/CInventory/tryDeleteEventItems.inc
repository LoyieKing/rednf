bool CInventory::tryDeleteEventItems(const std::vector<std::pair<int, int> >& items)
{
    // b28d-solo-CInventory：362/347，P167 MISSING ppEi×2 ⇒ N113 封顶。
    if (m_pParent->getGmQuestFlag())
    {
        return true;
    }
    int okCount = 0;
    if (items.size() == 0)
    {
        return false;
    }
    // b28d-37：同 tryInsertEventItems——P167 差集 MISSING ppEi/C1Ev/pair::C1Ev/aSEOS0_，
    // 按 P50 反例复活（先声明后赋值）+ P135（具名 pair 右值赋值）改形。
    // [结果] our 633→651（ORIG 675），仅剩 ppEi 残差 ⇒ N 封顶，语义闭合。
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
            if (use_money(amount, (eMoneySubReason)4, false))
            {
                okCount++;
            }
            it++;
            continue;
        }
        else if (itemIdx == 2)
        {
            if ((unsigned int)((CUserCharacInfo*)m_pParent)->GetWinPoint() >=
                (unsigned int)amount)
            {
                okCount++;
            }
            it++;
            continue;
        }
        else
        {
            // [实测·b28d-tu-CInventory] type 改 enum 在本件**零效**（差异 d=3 不变；
            // CalNeedSlot/CalBlankSlot 处有效是因那两件 type 是**块级声明+循环内赋值**）
            // ⇒ 保留 int 形，残余 1 条装载序差记 P75。
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
                        okCount++;
                        m_pEquipSlot[i].reset();
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
                            okCount++;
                            m_pEquipSlot[i].reset();
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
                            okCount++;
                            m_invenItem[i].reset();
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
                        if (m_pEquipSlot[i].m_addInfo2 == amount)
                        {
                            m_pEquipSlot[i].reset();
                        }
                        else
                        {
                            m_pEquipSlot[i].m_addInfo2 -= amount;
                        }
                        found = true;
                        okCount++;
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
                            if (m_pEquipSlot[i].m_addInfo2 == amount)
                            {
                                m_pEquipSlot[i].reset();
                            }
                            else
                            {
                                m_pEquipSlot[i].m_addInfo2 -= amount;
                            }
                            found = true;
                            okCount++;
                            break;
                        }
                    }
                }
            }
        }
        it++;
    }
    if (okCount != (int)items.size())
    {
        return false;
    }
    return true;
}