int CInventory::insert_event_items(const std::vector<std::pair<int, int> >& items,
                                   std::vector<int>& outSlots, eMoneyAddReason moneyReason,
                                   eItemAddReason itemReason, bool bLog,
                                   const char* a, const char* b)
{
    // b28d-solo-CInventory：871/874，N113（ppEi）封顶，语义闭合。
    // b28d-37 [N82/N89 封顶，ORIG 0x8506bb2]：P167 调用点差集 CLEAR（95/95）⇒ 语义闭合；871/874 差为帧/槽与模板内联。
    typedef std::vector<std::pair<int, int> >::const_iterator Iterator;
    int count = 0;
    if (items.size() == 0)
    {
        return count;
    }
    // ORIG 序言块序：pair 默认构造（+0x038）→ 迭代器默认构造（+0x046）→
    // it = items.begin()（+0x06c 赋值，非拷贝初始化）→ item（+0x078）→ totalValue=0。
    std::pair<int, int> cur;
    Iterator it;
    it = items.begin();
    Inven_Item item;
    int totalValue = 0;
    while (it != items.end())
    {
        item.reset();
        // ORIG +0x0bf：临时 pair 物化 + operator=(pair&&)（范式库:358）。
        cur = std::pair<int, int>(*it);
        int itemIdx = cur.first;
        int cnt = cur.second;
        if (itemIdx == 0)
        {
            // ---- 金钱条目 ----
            if (cnt > 499999 && m_pParent != 0)
            {
                cMyTrace trace(
                    __PRETTY_FUNCTION__,
                    0x1dc7, 4);
                trace("Quest reward money(%d) mid(%s) characno(%d) reason(%d)", cnt,
                      NumberToString(m_pParent->get_acc_id(), 0),
                      m_pParent->get_charac_no(-1), (int)moneyReason);
            }
            if (m_pParent != 0 && ((CUserCharacInfo*)m_pParent)->getCurCharacR() != 0)
            {
                // ORIG 0x08506dbd：gain 结果直接覆写 cnt（单槽 -0xe0），
                // 邮件差额判据用 cur.second - cnt。
                cnt = ((CUserCharacInfo*)m_pParent)->getCurCharacInvenW()->
                    gain_money(cnt, moneyReason, true, 0);
            }
            outSlots.push_back(0);
            outSlots.push_back(0);
            outSlots.push_back(cnt);
            ++count;
            it++;   // ORIG：迭代器先推进（+0x283），再判定邮件
            if ((int)moneyReason == 3)
            {
                totalValue += cnt;
            }
            if (cur.second > cnt && bLog)
            {
                char title[0x15];
                strncpy(title, g_scriptStringManager_.findString(4, a, (bool*)0), 0x14);
                char body[0x100];
                strncpy(body, g_scriptStringManager_.findString(4, b, (bool*)0), 0xff);
                WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
                    title, item, cur.second - cnt, m_pParent->getCurCharacNo(), body,
                    strlen(body), 0, m_pParent->GetServerGroup(), false, false);
            }
            continue;
        }

        CItem* pItem = G_CDataManager()->find_item(itemIdx);
        if (pItem == 0)
        {
            cMyTrace trace(
                __PRETTY_FUNCTION__,
                0x1e14, 5);
            trace("CInventory::insert_event_items, G_CDataManager()->find_item(%d)",
                  itemIdx);
            it++;
            continue;
        }

        item.m_addInfo = itemIdx;
        if (!pItem->is_stackable())
        {
            if (!pItem->IsAvatarItem())
            {
                if (pItem->IsCreatureItem() && pItem->GetItemType() == 0x16)
                {
                    // ---- 造物蛋 ----
                    if ((int)itemReason != 8)
                    {
                        continue;   // ORIG 怪癖：不推进迭代器
                    }
                    G_CDataManager()->m_itemList->create_item(
                        ENUM_ITEM_CREATE_TYPE_0, item, cnt);
                    if (item.m_field1 == 5)
                    {
                        if (!user_creature::CCreatureItemConverter::SetEggItem(&item))
                        {
                            continue;   // ORIG 怪癖：不推进迭代器
                        }
                        int r = insertItemIntoInventory(item, (eItemAddReason)8, true, false);
                        if (r >= 0)
                        {
                            int expirationDate = pItem->getExpirationDate();
                            int usablePeriod = pItem->getUsablePeriod();
                            GetCreatureMgrW()->InsertCreatureItem(&item, r, 8,
                                                                  usablePeriod,
                                                                  expirationDate);
                            ++count;
                            outSlots.push_back(r);
                            // ORIG +0xb68：item.m_addInfo 经临时物化走 push_back(int&&)
                            // （其 m_addInfo 为无符号型，本仓声明 int，以 +x 复现转换形态）。
                            outSlots.push_back(+item.m_addInfo);
                            outSlots.push_back(cnt);
                        }
                    }
                }
                else
                {
                    // ---- 普通非堆叠：逐份插入 ----
                    for (int k = 0; k < cnt; ++k)
                    {
                        pItem->make_item(item);
                        int r = insertItemIntoInventory(item, itemReason, true, true);
                        if (r == -1)
                        {
                            if (!bLog)
                            {
                                break;   // ORIG：跳出循环并走共用尾推进
                            }
                            char title[0x15];
                            strncpy(title, g_scriptStringManager_.findString(4, a, (bool*)0),
                                    0x14);
                            char body[0x100];
                            strncpy(body, g_scriptStringManager_.findString(4, b, (bool*)0),
                                    0xff);
                            WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
                                title, item, 0, m_pParent->getCurCharacNo(), body,
                                strlen(body), 0, m_pParent->GetServerGroup(), false, false);
                            continue;   // 尝试下一份
                        }
                        ++count;
                        outSlots.push_back(r);
                        outSlots.push_back(+item.m_addInfo);
                        outSlots.push_back(item.m_addInfo2);
                        if ((int)itemReason == 8)
                        {
                            int price = 0;
                            G_Store()->GetSellItemPrice(item, pItem,
                                                        (short)item.m_addInfo2, false,
                                                        price);
                            totalValue += price;
                        }
                    }
                }
            }
            else
            {
                // ---- Avatar 物品 ----
                if ((int)itemReason != 8)
                {
                    continue;   // ORIG 怪癖：不推进迭代器
                }
                char ipg[0x18];
                WongWork::CGenUniqueNo::genIPGNo(
                    (WongWork::CGenUniqueNo::ENUM_IPGNO_TYPE)2,
                    (unsigned int)((CUserCharacInfo*)m_pParent)->getCurCharacNo(), ipg);
                int usablePeriod = pItem->getUsablePeriod();
                int r = AddAvatarItem(itemIdx, usablePeriod, false, 0, (char)-1, ipg,
                                      (eAvatarItemAddReason)8, false, 0);
                if (r == -1)
                {
                    if (!bLog)
                    {
                        goto finish_value_statistic;   // ORIG：跳到函数尾统计
                    }
                    WongWork::CMailBoxHelper::ReqDBSendNewAvatarMail(
                        m_pParent, ((CUserCharacInfo*)m_pParent)->getCurCharacNo(), itemIdx,
                        0, '\0', (char)-1, ipg, (char*)0, 0);
                    it++;
                }
                else
                {
                    ++count;
                    outSlots.push_back(r);
                    outSlots.push_back(+item.m_addInfo);
                    int nowTick = GlobalData::s_systemTime_.getCurSec();
                    // ORIG +0x974：GetRemainDate 返回值直接作实参（右值 → int&&）。
                    outSlots.push_back(GetAvatarItemMgrR()->GetRemainDate(
                        GetInvenSlot(2, r).m_addInfo2, nowTick));
                }
            }
        }
        else
        {
            // ---- 堆叠物品 ----
            item.m_addInfo2 = cnt;
            pItem->make_item(item);
            int r = insertItemIntoInventory(item, itemReason, true, true);
            if (r == -1)
            {
                if (!bLog)
                {
                    goto finish_value_statistic;   // ORIG：跳到函数尾统计
                }
                char title[0x15];
                strncpy(title, g_scriptStringManager_.findString(4, a, (bool*)0), 0x14);
                char body[0x100];
                strncpy(body, g_scriptStringManager_.findString(4, b, (bool*)0), 0xff);
                WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
                    title, item, 0, m_pParent->getCurCharacNo(), body, strlen(body), 0,
                    m_pParent->GetServerGroup(), false, false);
                it++;
            }
            else
            {
                ++count;
                outSlots.push_back(r);
                outSlots.push_back(+item.m_addInfo);
                outSlots.push_back(item.m_addInfo2);
                if ((int)itemReason == 8)
                {
                    if (m_pParent != 0)
                    {
                        GetInstanceCubeStatistic()->collectCubeStatistics(
                            pItem, item.m_addInfo2, m_pParent, (CUBE_STATISTIC_FIELD)4);
                    }
                    int price = 0;
                    G_Store()->GetSellItemPrice(item, pItem, (short)item.m_addInfo2, false,
                                                price);
                    totalValue += price;
                }
            }
        }
        // ORIG 共用尾 it++（+0xe7d）：堆叠/头像/造物蛋/非堆叠各成功路径汇流。
        it++;
    }

finish_value_statistic:
    if (((int)itemReason == 8 || (int)moneyReason == 3) && m_pParent != 0)
    {
        GetInstanceValueStatistic()->AddValueStatistic((VALUE_STATISTIC_FIELD)0xa,
                                                       m_pParent, totalValue);
    }
    return count;
