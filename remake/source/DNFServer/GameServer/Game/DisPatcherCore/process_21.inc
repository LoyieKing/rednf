int DisPatcher_BuyItem::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    BuyItemParam& data = static_cast<BuyItemParam&>(param);
    MSG_BUY_ITEM& req = *reinterpret_cast<MSG_BUY_ITEM*>(&msg);

    int errNo = check_error(user, msg);
    if (errNo > 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x15, (unsigned char)errNo);
        return -1;
    }
    if (errNo < 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x15, 0x15);
        return LineFunc(0x1a1,
                        __PRETTY_FUNCTION__,
                        0, user->get_acc_id());
    }
    errNo = get_data(user, req, data);
    if (errNo != 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x15, (unsigned char)errNo);
        return -1;
    }
    if (data.m_pair.first == 0x29f2a7)
    {   // 死亡塔专属商品：须非交易中且已开通对应死亡塔
        if (user->get_state() == 3)
        {
            return LineFunc(0x1b4,
                            __PRETTY_FUNCTION__,
                            0, 0);
        }
        if (G_CGameManager()->getBossTower(user->getBossTowerIndex()) == 0)
        {
            return LineFunc(0x1b8,
                            __PRETTY_FUNCTION__,
                            0, 0);
        }
    }
    data.m_item.m_addInfo = req.m_itemIdx;      // ORIG *(int*)(param+6)=*(int*)(msg+0xd)
    data.m_a8->make_item(data.m_item);          // vtable slot +0x08
    if (!data.m_a8->IsCreatureItem() && !data.m_a8->IsAvatarItem())
    {   // 非生物/非时装：数量取自 msg+0x11（写 m_item.m_addInfo2 @param+0xb）
        data.m_item.m_addInfo2 = req.m_buyCount;
    }
    data.m_a0 = user->getCurCharacInvenW()->insertItemIntoInventory(
        data.m_item, (eItemAddReason)0, true, false);
    if (data.m_a0 < 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x15, 4);
        return -1;
    }
    CDataManager* dataMgr = G_CDataManager();
    CItem* shopItem = dataMgr->find_item(data.m_item.m_addInfo);
    if (shopItem == 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x15, 0x13);
        return -1;
    }
    if (shopItem->IsCreatureItem() && shopItem->GetItemType() == 0x16)
    {   // 生物物品：按有效期限写入生物装备栏
        int expirationDate = shopItem->getExpirationDate();
        int usablePeriod = shopItem->getUsablePeriod();
        user_creature::CCreatureMgr* creatureMgr =
            user->getCurCharacInvenW()->GetCreatureMgrW();
        creatureMgr->InsertCreatureItem(&data.m_item, data.m_a0, 0,
                                        usablePeriod, expirationDate);
    }
    CGuildServerProxy* guildProxy =
        GlobalData::s_guild_proxy_mgr->GetServerProxy(user->GetServerGroup());
    guildProxy->OnBuyItem(user, data.m_a8,
                          (unsigned int)data.m_90, (unsigned int)data.m_94);
    user->getCurCharacInvenW()->use_money(data.m_80, (eMoneySubReason)0, true);
    if (data.m_pair.first != -1)
    {   // 消耗材料（如代币/兑换券）：delete_item(INVEN_TYPE 1, 槽, 数量, 0x1a, log)
        user->getCurCharacInvenW()->delete_item((INVEN_TYPE)1, data.m_a4,
                                                data.m_pair.second,
                                                (eItemDelReason)0x1a, true);
    }
    _onBuyItemEvent(data, user);
    statistic_process(user, data);
    HistoryLog::WriteBuyItem((FILE*)user->m_field796f8,
                             data.m_item.m_addInfo, data.m_item.m_addInfo2);
    user->SendAradEventItemLog((unsigned long)data.m_item.m_addInfo,
                               data.m_item.m_addInfo2);
    user->eventDeliveryBuyItem(req.m_deliveryType, data.m_item);
    return 0;
