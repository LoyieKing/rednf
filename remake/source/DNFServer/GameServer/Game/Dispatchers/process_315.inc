int Dispatcher_MonstercardBind::process(CUser* user, MSG_BASE& msg,
                                        ParamBase& param)
{
    stMonstercardBind& res = static_cast<stMonstercardBind&>(param);
    MSG_MONSTERCARD_BIND& m = reinterpret_cast<MSG_MONSTERCARD_BIND&>(msg);

    res.m_4 = check_error(user, msg);
    if (res.m_4 > 0)
    {
        return 0;
    }
    if (res.m_4 < 0)
    {
        return LineFunc(0x44db,
                        "virtual int Dispatcher_MonstercardBind::process(CUser*, "
                        "MSG_BASE&, ParamBase&)",
                        res.m_4, 0);
    }

    // ORIG：每次 GetInvenSlot 前重新 getCurCharacInvenR()（不缓存指针）
    Inven_Item slotA = user->getCurCharacInvenR()->GetInvenSlot(1, m.m_0d);
    CItem* cardA = G_CDataManager()->find_item(slotA.m_addInfo);
    Inven_Item slotB = user->getCurCharacInvenR()->GetInvenSlot(1, m.m_0f);
    CItem* cardB = G_CDataManager()->find_item(slotB.m_addInfo);
    Inven_Item slotC = user->getCurCharacInvenR()->GetInvenSlot(1, m.m_11);
    CItem* cardC = G_CDataManager()->find_item(slotC.m_addInfo);

    std::vector<std::pair<int, int> > results;
    results.clear();   // ORIG 0x081d8a32：构造后显式 clear

    bool bindOk = false;
    {
        // ---- get_bind_result @0x081d84a0 T：主卡绑定率 → 轮盘选表 ----
        const int rarity = cardB->get_rarity();
        bindOk = (get_bind_result(user, (short)m.m_0d, (ENUM_RARITY)rarity,
                                  results) != 0);
    }

    if (!bindOk)
    {
        res.m_4 = 0x13;
        return 0;
    }

    // 影子背包试删试插（失败不影响真背包）
    CInventory tmpInv;
    InventoryMemory tmpMem;
    tmpInv.SetInventoryMemory(&tmpMem);
    // ORIG 081d8ac2：实参为 getCurCharacInvenRefR()(W 0x081803ca，charac+0xda
    // 槽区视图)，直接按 CInventory 传入 setCopy —— 类型双关，忠实转写。
    tmpInv.setCopy(*reinterpret_cast<const CInventory*>(
        user->getCurCharacInvenRefR()));

    const bool delA = tmpInv.delete_item((INVEN_TYPE)1, m.m_0d, 1,
                                         (eItemDelReason)0x2c, false);
    if (!delA)
    {
        res.m_4 = 0x11;
        return 0;
    }
    const bool delB = tmpInv.delete_item((INVEN_TYPE)1, m.m_0f, 1,
                                         (eItemDelReason)0x2c, false);
    if (!delB)
    {
        res.m_4 = 0x11;
        return 0;
    }
    const bool delC = tmpInv.delete_item((INVEN_TYPE)1, m.m_11, 1,
                                         (eItemDelReason)0x2c, false);
    if (!delC)
    {
        res.m_4 = 0x11;
        return 0;
    }
    if ((int)tmpInv.tryInsertEventItems(results) != (int)results.size())
    {
        res.m_4 = 0x11;
        return 0;
    }

    // 真背包扣三张卡并插入绑定结果
    CInventory* invenW = user->getCurCharacInvenW();
    invenW->delete_item((INVEN_TYPE)1, m.m_0d, 1, (eItemDelReason)0x2c, true);
    invenW->delete_item((INVEN_TYPE)1, m.m_0f, 1, (eItemDelReason)0x2c, true);
    invenW->delete_item((INVEN_TYPE)1, m.m_11, 1, (eItemDelReason)0x2c, true);

    std::vector<int> outSlots;
    outSlots.clear();   // ORIG 0x081d8cdb：构造后显式 clear
    invenW->insert_event_items(results, outSlots, (eMoneyAddReason)0,
                               (eItemAddReason)0x34, false, "", "");

    res.m_8 = m.m_0d;
    res.m_a = m.m_0f;
    res.m_c = m.m_11;
    res.m_list = outSlots;
    return 0;
