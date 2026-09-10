int Dispatcher_SeriaRoom_Ani_Deco_Event::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    MSG_SERIAROOM_DECO_EVENT& req = reinterpret_cast<MSG_SERIAROOM_DECO_EVENT&>(msg);
    stSeriaRoomDecoEvent& res = static_cast<stSeriaRoomDecoEvent&>(param);
    int err = check_error(user, msg);
    res.m_4 = err;
    if (err > 0)
    {
        return 0;
    }
    if (err < 0)
    {
        return LineFunc(
            0x6551,
            __PRETTY_FUNCTION__,
            err, 0);
    }

    seriaRoom_AniDeco info = user->getSeriaRoomAniDecoInfo();   // +0x08d（4 字节值返回）

    // 进度上限：info.m_field2(有符号 word) >= maxAniDecoItem() → 0x14
    // （ORIG cmp %ax,%bx / setge：双方取低 16 位有符号比较）
    if (static_cast<short>(info.m_field2) >=
        static_cast<short>(G_CDataManager()->maxAniDecoItem()))
    {
        res.m_4 = 0x14;
        return 0;
    }

    // 客户端指定槽位 = msg+0xe word（即 read 写入的 req.m_pad+1）
    unsigned short slot =
        req.m_decoEventId;

    const CInventory* invenR = user->getCurCharacInvenR();
    Inven_Item slotItem = invenR->GetInvenSlot(static_cast<int>(INVEN_TYPE(1)),
                                               static_cast<int>(slot));

    // find_item(item idx) 缺定义 → 1
    CItem* itemDef = G_CDataManager()->find_item(slotItem.m_addInfo);
    if (itemDef == 0)
    {
        res.m_4 = 1;
        return 0;
    }

    // info.m_field2 == -1 哨兵 → 1（ORIG movzwl -0x2e / cmpw $0xffff）
    if (info.m_field2 == -1)
    {
        res.m_4 = 1;
        return 0;
    }

    // 槽位有效性校验：(idx=info.m_field2, value=itemIdx)；假 → 0x17
    // （ORIG xor $1,%eax：真继续、假报错）
    if (!G_CDataManager()->invalidAniDecoItem(
            static_cast<int>(static_cast<short>(info.m_field2)), slotItem.m_addInfo))
    {
        res.m_4 = 0x17;
        return 0;
    }

    // 成功路径：回显槽位 → 删除道具 → 进度+1 → 落库 → 事件处理
    res.m_8 = static_cast<short>(slot);                         // +0x18d

    CInventory* invenW = user->getCurCharacInvenW();
    invenW->delete_item(INVEN_TYPE(1), static_cast<int>(slot), 1,
                        static_cast<eItemDelReason>(3), true);

    seriaRoom_AniDeco updated = info;                           // m_field0 不变
    updated.m_field2 =
        static_cast<short>(static_cast<unsigned short>(info.m_field2 + 1));
    user->setSeriaRoomAniDecoInfo(updated);

    DB_UpdateSeriaRoomAnimationDeco::makeRequest(
        user->get_acc_id(), user->GetServerGroupToPvP(),
        user->getCurCharacNo(), info.m_field0, updated.m_field2);

    CEventManager* eventMgr = GlobalData::s_event_manager;
    CEventBase* evt = eventMgr->GetRepeatEvent(0x69);
    if (evt != 0 && evt->IsEventing(0))                         // vtable +0x34 槽
    {
        CEventBase* aniDecoEvent = eventMgr->GetRepeatEvent(0x69);
        if (aniDecoEvent != 0)
        {
            res.m_4 = static_cast<SeriaRoomAniDecoEvent*>(aniDecoEvent)
                          ->processEvent(*user);
        }
    }
    return 0;
