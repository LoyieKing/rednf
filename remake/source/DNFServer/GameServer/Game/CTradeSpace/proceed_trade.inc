bool CTradeSpace::proceed_trade()
{
    // b28d-35: DIFF→N82（1606/1618；§6.2）。ORIG @0x0853087a。
    if (!IsLocked())
        return false;
    if (!checkTrade())
        return false;

    int tradeLogMoney[2] = {0, 0};                // -0x88/-0x84（TradeEnd 用，恒 0）
    int avgPrice[2] = {0, 0};                     // -0x90/-0x8c（拍卖均价累计）
    PacketGuard packet[2];                        // -0xa8 起 2 个
    std::map<CUser*, std::vector<int> > updateMap;  // -0xc4

    // ---- 第一段：防 hack 上报（trader 0..1）----
    for (int trader = 0; trader <= 1; ++trader)
    {
        CUser* src = m_pTraders[trader];          // -0x60
        CUser* dst = m_pTraders[1 - trader];      // -0x5c
        char ip[16];                              // -0xd4
        ((XNuclear::CHades*)dst->getHades())->GetIP(ip);
        int srcMoney = src->getCurCharacInvenR()->get_money();
        // ORIG +0x14b-0x1b2：srcMoney→money→accId→receiver 求值序
        src->getAcccountHackInfo()->sendGold(0, dst->get_acc_id(),
                                             m_nMoney[trader], srcMoney, 0, ip);
        for (int slot = 3; slot <= 0x1a; ++slot)
        {
            // ORIG +0x1c0：遍历 (1-trader) 行、发方 src 上报（0x8530a3a）
            int itemIdx = m_slotsData[1 - trader][slot].m_item.m_addInfo;
            if (src->getAcccountHackInfo()->isAvatar((unsigned)itemIdx)) {
                unsigned long acc = dst->get_acc_id();
                src->getAcccountHackInfo()->sendAvatar(0, acc);
            }
        }
    }

    // ---- 第二段：金币转移 + 物品转移（trader 0..1）----
    for (int trader = 0; trader <= 1; ++trader)
    {
        int count = 0;
        packet[trader].clear();
        packet[trader].put_header(0, 0x12);
        for (int slot = 3; slot <= 0x1a; ++slot)
        {
            if (m_slotsData[trader][slot].m_item.m_addInfo != 0)
                ++count;
        }
        if (m_nMoney[trader] > 0)
        {
            ++count;
            packet[trader].put_short(count);
            packet[trader].put_short(0);
            packet[trader].put_short(0);
            if (m_pTraders[1 - trader])
                m_pTraders[1 - trader]->getCurCharacInvenW()->gain_money(
                    m_nMoney[trader], (eMoneyAddReason)1, 1, 0);
            if (m_nMoney[trader] % 10000 != 0)
            {
                unsigned int etc = m_pTraders[1 - trader]->SetETC(
                    0x2c4, m_pTraders[trader]->get_acc_id());
                m_pTraders[1 - trader]->getHackAnalyzer()
                    ->addServerHackCnt(m_pTraders[1 - trader],
                                       (WongWork::ENUM_HACKTYPE)0x2c4, 1, etc,
                                       0);
            }
            if ((m_pTraders[trader]->isHackUser() ||
                 m_pTraders[trader]->isHackUserTimer()) &&
                m_nMoney[trader] != 0)
                m_pTraders[1 - trader]->gainMoneyFromHackUser(
                    m_pTraders[trader]->get_acc_id(),
                    (unsigned long long)(long long)m_nMoney[trader], 1, 0);
            if (m_pTraders[trader] && m_pTraders[1 - trader] &&
                m_pTraders[trader]->getCurCharacMoney() == 0)
            {
                CUser* to = m_pTraders[1 - trader];   // -0x50
                CUser* from = m_pTraders[trader];     // -0x4c
                CStreamGuard guard(
                    GlobalData::s_stream_pool->Acquire("item_trade.cpp",
                                                       0xc51),
                    true);
                guard.operator*()->operator<<(0xa1);
                guard.operator*()->operator<<(-1);
                SIG_SAVE_GOLD_ZERO_TRADE_LOG* log =
                    guard->GetInBuffer<SIG_SAVE_GOLD_ZERO_TRADE_LOG>();
                log->m_to_m_id = (unsigned)to->get_acc_id();
                std::strncpy(log->m_to_nat_ip, to->m_network.get_str_ip(),
                             0x10);
                to->m_network.GetPeerIP2(log->m_to_peer_ip, 0x14);
                log->m_to_nat_type = to->m_network.get_nat_type();
                log->m_from_m_id = (unsigned)from->get_acc_id();
                std::strncpy(log->m_from_nat_ip, from->m_network.get_str_ip(),
                             0x10);
                from->m_network.GetPeerIP2(log->m_from_peer_ip, 0x14);
                log->m_from_nat_type = from->m_network.get_nat_type();
                std::strncpy(log->m_to_mac_addr, to->m_network.get_mac_addr(),
                             0x41);
                std::strncpy(log->m_from_mac_addr,
                             from->m_network.get_mac_addr(), 0x41);
                log->m_injury_money = (unsigned)m_nMoney[trader];
                GlobalData::s_msgq_mgr->put(MsgQueueMgr::LOGDB_Q, guard);
            }
            m_nMoney[trader] = 0;
            // ORIG 0x853087a：WriteTradeUp/Down 第三参在清零后读成员（恒 0）
            HistoryLog::WriteTradeUp(
                (FILE*)m_pTraders[trader]->m_field796f8, 0,
                m_nMoney[trader]);
            HistoryLog::WriteTradeDown(
                (FILE*)m_pTraders[1 - trader]->m_field796f8, 0,
                m_nMoney[trader]);
        }
        else
        {
            packet[trader].put_short(count);
        }

        for (int slot = 3; slot <= 0x1a; ++slot)
        {
            if (m_pTraders[1 - trader] &&
                m_slotsData[trader][slot].m_item.m_addInfo != 0)
            {
                int ret = 0;   // ORIG -0xac 初值 0（+0xde），三分支共享
                // 拍卖均价累计（ORIG +0x91d；-0x3c item ptr 缓存形）
                CAuctionAveragePrice* avgMgr =
                    G_CGameManager()->GetAuctionAveragePriceManager();
                if (avgMgr)
                {
                    Inven_Item* item = &m_slotsData[trader][slot].m_item;
                    unsigned char upgrade = item->GetUpgrade();
                    int avg = avgMgr->GetAveragePrice(item->m_addInfo,
                                                      upgrade);
                    if (item->m_field1 == 2 || item->m_field1 == 3)
                        avg *= item->m_addInfo2;
                    avgPrice[trader] += avg;
                }
                if (m_slotsData[trader][slot].m_item.m_field1
                        == Inven_Item::ITEM_TYPE_AVATAR)  // 头像
                {
                    ret = m_pTraders[1 - trader]->getCurCharacInvenW()
                              ->insertAvatarIntoInventory(
                                  m_slotsData[trader][slot].m_item,
                                  (eAvatarItemAddReason)1, false);
                    if (ret >= 0)
                    {
                        TradeSlot* itemTs = &m_slotsData[trader][slot];  // -0x34
                        _SaveAvatarItemOwnerChange(
                            itemTs->m_item.m_addInfo2, itemTs->m_field04,
                            m_pTraders[1 - trader]->getCurCharacNo(),
                            (unsigned char)ret);
                        DB_InsertCeraItemHistory::makeRequest(
                            itemTs->m_item.m_addInfo2, itemTs->m_field04,
                            m_pTraders[1 - trader]->getCurCharacNo(), 2);
                        // Erase 于发方 mgr、Add 于收方 mgr（ORIG
                        // 0x8531400/0x8531443）；uid 两次重读（+0x0b74/+0x0ba5）
                        WongWork::Avatar_Item* av =
                            m_pTraders[trader]->getCurCharacInvenW()
                                ->GetAvatarItemMgrW()
                                ->EraseAvatarItem(
                                    itemTs->m_item.m_addInfo2);
                        m_pTraders[1 - trader]->getCurCharacInvenW()
                            ->GetAvatarItemMgrW()
                            ->AddAvatarItem(itemTs->m_item.m_addInfo2, av);
                        // 两日志各调一次 GetIPGAgencyNo（P48 无 CSE）
                        m_pTraders[trader]->m_historyLog.AvatarItemDel(
                            itemTs->m_item.m_addInfo,
                            itemTs->m_item.m_addInfo2,
                            av->GetIPGAgencyNo(), (eAvatarItemDelReason)0);
                        m_pTraders[1 - trader]->m_historyLog.AvatarItemAdd(
                            itemTs->m_item.m_addInfo,
                            itemTs->m_item.m_addInfo2,
                            av->GetIPGAgencyNo(), (eAvatarItemAddReason)1);
                    }
                }
                else if (m_slotsData[trader][slot].m_field08 == 7)  // 生物
                {
                    ret = m_pTraders[1 - trader]->getCurCharacInvenW()
                              ->insertItemIntoInventory(
                                  m_slotsData[trader][slot].m_item,
                                  (eItemAddReason)0x1a, false, true);
                    if (ret >= 0)
                    {
                        if (m_slotsData[trader][slot].m_item.m_field1
                                == Inven_Item::ITEM_TYPE_CREATURE_EQUIP)
                        {
                            if (m_pTraders[trader]->getCurCharacInvenW()
                                     ->GetCreatureMgrW()
                                     ->FindCreatureItem(m_slotsData[trader]
                                                        [slot].m_item
                                                            .m_addInfo2) == 0)
                                return false;
                        }
                        int characNo =
                            m_pTraders[trader]->getCurCharacR()->m_characNo;
                        int size =
                            m_pTraders[trader]->getCurCharacInvenW()
                                ->GetCreatureMgrR()->GetCreatureItemSize() +
                            m_fieldF68[trader];
                        m_pTraders[1 - trader]->getCurCharacInvenW()
                            ->GetCreatureMgrW()
                            ->ChangeOwner(&m_slotsData[trader][slot].m_item,
                                          ret, 0x1a, characNo, size);
                        m_fieldF68[1 - trader] += 1;
                        m_pTraders[trader]->getCurCharacInvenW()
                            ->GetCreatureMgrW()
                            ->DeleteCreatureItem(
                                m_slotsData[trader][slot].m_item
                                    .get_add_info(),
                                0x14);
                        m_pTraders[trader]->m_historyLog.CreatureItemDel(
                            (INVEN_TYPE)3,
                            m_slotsData[trader][slot].m_item.m_addInfo,
                            m_slotsData[trader][slot].m_item.get_add_info(),
                            0, m_slotsData[trader][slot].m_item.m_field1,
                            (eItemDelReason)0x14);
                    }
                }
                else  // 普通物品
                {
                    if (IsTradeLimitAttachTypeItem(
                            m_slotsData[trader][slot].m_item))
                    {
                        DecreaseTradeLimitCount(
                            m_slotsData[trader][slot].m_item);
                        // ORIG +0x1090：记发送方 trader 的 historyLog
                        m_pTraders[trader]->m_historyLog.ChangeItemLimitCount(
                            1, slot,
                            m_slotsData[trader][slot].m_item.m_addInfo, -1,
                            2);
                    }
                    ret = m_pTraders[1 - trader]->getCurCharacInvenW()
                              ->insertItemIntoInventory(
                                  m_slotsData[trader][slot].m_item,
                                  (eItemAddReason)1, true, true);
                    updateMap[m_pTraders[1 - trader]].push_back(ret);
                }

                // 共享负值检查（ORIG +0x11ce）
                if (ret < 0)
                {
                    cMyTrace(__PRETTY_FUNCTION__, 0xce5, 5)(
                        "insert_item(INVEN_TYPE_INVENTORY) fail - fatal! %d",
                        m_pTraders[1 - trader]->getCurCharacNo());
                    m_slotsData[trader][slot].m_item.reset();
                    return false;
                }
                HistoryLog::WriteTradeUp(
                    (FILE*)m_pTraders[trader]->m_field796f8,
                    m_slotsData[trader][slot].m_item.m_addInfo,
                    m_slotsData[trader][slot].m_item.get_add_info());
                HistoryLog::WriteTradeDown(
                    (FILE*)m_pTraders[1 - trader]->m_field796f8,
                    m_slotsData[trader][slot].m_item.m_addInfo,
                    m_slotsData[trader][slot].m_item.get_add_info());
                m_slotsData[trader][slot].m_item.reset();
                packet[trader].put_short(slot);
                packet[trader].put_short(ret);
            }
        }
        packet[trader].finalize(true);
    }

    // ---- 第三段：发包 + 交易计数 ----
    for (int trader = 0; trader <= 1; ++trader)
    {
        m_pTraders[1 - trader]->Send(packet[trader]);
        m_pTraders[1 - trader]->incTradeCount();
    }

    // ---- 第四段：更新物品列表 ----
    for (std::map<CUser*, std::vector<int> >::iterator it =
             updateMap.begin();
         it != updateMap.end(); ++it)
    {
        CUser* user = (*it).first;
        if (!user)
            return false;
        std::vector<int>& vec = (*it).second;
        for (std::vector<int>::iterator vit = vec.begin(); vit != vec.end();
             ++vit)
        {
            user->SendUpdateItemList((CUser::eSendTarget)1,
                                     (ENUM_ITEMSPACE)0, *vit);
        }
    }

    for (int trader = 0; trader <= 1; ++trader)
    {
        m_pTraders[trader]->m_historyLog.TradeEnd(
            (eTradeEndReason)2, avgPrice[trader], tradeLogMoney[trader],
            avgPrice[1 - trader], tradeLogMoney[1 - trader]);
        // b28d-solo-CTradeSpace：ORIG 在调用点内联 GetGoldControl（无 call，
        //   直接 lea +0x8e980）；4.4 -O0 不内联头内 inline ⇒ 写具名成员访问
        //   复现 ORIG 形（P119 反向：调用点展开）。
        m_pTraders[trader]->m_goldControl.TradeComplete(
            tradeLogMoney[trader], tradeLogMoney[1 - trader]);
    }
    return true;
