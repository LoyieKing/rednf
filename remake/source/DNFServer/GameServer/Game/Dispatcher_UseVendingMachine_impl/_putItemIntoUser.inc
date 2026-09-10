void Dispatcher_UseVendingMachine::_putItemIntoUser(
    CUser* user, unsigned long itemIdx, unsigned int count,
    stPutItemResult_t& result) const
{
    // 常量（ORIG 直接 cmpl 立即数比较，避免局部 const 数组）
    enum { PAY_MONEY = 0, PAY_EVENTCOIN = 1, PAY_WINPOINT = 2 };
    // （其它值 = 给实物物品，按 itemIdx）

    result.m_itemIdx = itemIdx;
    result.m_count   = count;
    result.m_result  = 0;
    result.m_slot    = -1;
    result.m_success = 1;

    if (result.m_itemIdx == 0xffffffffUL)
        return;

    short slot = -1;

    if (itemIdx == PAY_MONEY)
    {
        static_cast<CUserCharacInfo*>(user)->getCurCharacInvenW()->gain_money(
            (int)count, (eMoneyAddReason)0x15, 1, 0);
        slot = 0;
    }
    else if (itemIdx == PAY_EVENTCOIN)
    {
        static_cast<CUserCharacInfo*>(user)->getCurCharacInvenW()->SetEventCoin(
            static_cast<CUserCharacInfo*>(user)->getCurCharacInvenW()->GetEventCoin()
                + (int)count);
        // ORIG: cUserHistoryLog::EventCoinAdd((cUserHistoryLog*)(user+0x79700),coin,amount,6)
        user->m_historyLog.EventCoinAdd(
            (int)static_cast<CUserCharacInfo*>(user)->getCurCharacInvenR()->GetEventCoin(),
            (int)count, (eCoinAddReason)6);
        slot = 1;
    }
    else if (itemIdx == PAY_WINPOINT)
    {
        // ORIG _ZN5CUser12gainWinPointEi12eWPAddReason @0x0864fd2c（CUser 域已补声明+实现）。
        user->gainWinPoint((int)count, (eWPAddReason)3);
        slot = 2;
    }
    else
    {
        // 实物物品
        CStackableItem* item = static_cast<CStackableItem*>(
            G_CDataManager()->find_item((int)itemIdx));
        if (item == 0)
        {
            result.m_success = 0;
            return;
        }

        Inven_Item outItem;
        outItem.m_addInfo = itemIdx;
        G_CDataManager()->m_itemList->create_item((ENUM_ITEM_CREATE_TYPE)0, outItem,
                                                  (int)count);

        if (item->GetItemType() == 0x17)
        {
            // 需要全局效果：按 m_vec280（vector<stGlobalEffect_t>）逐项注册。
            // ORIG：registNewEffect(m_field0, m_field4, characName, 5, m_field8)。
            const std::vector<stGlobalEffect_t> eff = item->getGlobalEffectData();
            for (std::vector<stGlobalEffect_t>::const_iterator it = eff.begin(),
                     e = eff.end(); it != e; ++it)
            {
                GlobalData::s_globalEffectManager_->registNewEffect(
                    it->m_field0, it->m_field4,
                    static_cast<CUserCharacInfo*>(user)->getCurCharacName(), 5,
                    (long)it->m_field8);
            }
        }
        else if (item->IsAvatarItem())
        {
            char ipgBuf[0x18];
            slot = (short)static_cast<CUserCharacInfo*>(user)->getCurCharacInvenW()
                       ->AddAvatarItem((int)itemIdx, item->getUsablePeriod(), /*b*/0, /*c*/0,
                                       /*d*/(char)-1,
                                       WongWork::CGenUniqueNo::genIPGNo(
                                           (WongWork::CGenUniqueNo::ENUM_IPGNO_TYPE)4,
                                           static_cast<CUserCharacInfo*>(user)->getCurCharacNo(),
                                           ipgBuf),
                                       (eAvatarItemAddReason)0xe,
                                       /*e*/0, /*f*/(unsigned short)0);
            result.m_result = 1;
        }
        else if (item->IsCreatureItem() && item->GetItemType() == 0x16)
        {
            // 生物（蛋/宠物），先 SetEggItem 再 insert
            if (outItem.m_field1 == 5 /* ITEM_TYPE_CREATURE_EQUIP */)
            {
                if (!user_creature::CCreatureItemConverter::SetEggItem(&outItem))
                    return;
                slot = (short)static_cast<CUserCharacInfo*>(user)->getCurCharacInvenW()
                           ->insertItemIntoInventory(outItem, (eItemAddReason)0x23, 1, 0);
                if (slot >= 0)
                {
                    static_cast<CUserCharacInfo*>(user)->getCurCharacInvenW()
                        ->GetCreatureMgrW()->InsertCreatureItem(
                            &outItem, (int)slot, 0x23, item->getUsablePeriod(),
                            item->getExpirationDate());
                }
                // ORIG 0x0821bc0d：仅 SetEggItem 成功后才标记结果类型 7；
                // 转换失败或非 creature equipment 直接沿用默认结果类型 0。
                result.m_result = 7;
            }
        }
        else
        {
            // 普通实物
            // ORIG 0x0821bc1c 起：Cera 特殊物品处理 + 受限商品（防 hack）完整路径。
            //   IsSpecialItem → 非 0x10（普通 stackable）时 ProcessSpecialItem 分发，
            //   restrictCode 非 0 且未受限 → SetRestrictedGoods + SaveFeaturedIdx +
            //   SendRestrictedGoods；ProcessSpecialItem 返回 true 且 rc!=0 表示特殊
            //   处理已发物，直接结束（result 保持默认）。
            int specialType = 0, specialValue = 0;
            bool specialFlag = false;
            bool specialHandled = false;   // ORIG -0x21(%ebp)
            if (GlobalData::s_pCeraShop->IsSpecialItem((int)outItem.m_addInfo, specialType,
                                                       specialValue, specialFlag)
                && item->GetItemType() != 0x10)
            {
                std::vector<Inven_Item> items;    // ORIG -0x70(%ebp)
                int rc = 0;                       // ORIG -0x74(%ebp)
                int restrictCode =
                    G_CDataManager()->m_itemList->GetRestrictCode(
                        (int)outItem.m_addInfo);
                long curSec = GlobalData::s_systemTime_.getCurSec();
                WongWork::CCeraShop::stCeraShopItemParam_t param;  // ORIG -0x8c(%ebp)
                if (GlobalData::s_pCeraShop->ProcessSpecialItem(
                        user, (int)outItem.m_addInfo, restrictCode, rc, curSec, true,
                        items, param, 0, 0, false, 1)
                    && rc != 0)
                    return;   // ORIG：特殊处理已放物品，直接结束
                if (restrictCode != 0 && !user->IsRestrictedGoods(restrictCode))
                {
                    user->SetRestrictedGoods(restrictCode);
                    GlobalData::s_pCeraShop->SaveFeaturedIdx(user, restrictCode,
                                                             (int)curSec, 0);
                    user->SendRestrictedGoods(true, restrictCode);
                }
                specialHandled = true;   // ORIG：跳过普通 insert
            }
            if (!specialHandled)
            {
                slot = (short)static_cast<CUserCharacInfo*>(user)->getCurCharacInvenW()
                           ->insertItemIntoInventory(outItem, (eItemAddReason)0x23, 1, 0);
                if (slot == -1)
                    result.m_success = 0;
            }
        }
    }

    result.m_slot = slot;
    if (result.m_success != 0)
    {
        GetInstanceVendingMachineStatistic()->addOutputItem(result.m_itemIdx,
                                                            result.m_count);
    }
