int Dispatcher_LimitNpcBuyItem::check_error(CUser* user, MSG_BASE& msg)
{
    // ORIG 0x081e1172（docs/class_func_reports/Dispatcher_LimitNpcBuyItem/check_error.md）
    if (user == 0)
        return -1;
    // +0x1b：state<3 或 charac 空 → -1
    if (user->get_state() < 3 || user->getCurCharacR() == 0)
        return -1;

    // +0x54：msg+0x0d / msg+0x11 两字段（ORIG 无具名证据，保留裸偏移）
    unsigned int msgD = *(unsigned int*)((char*)&msg + 0xd);
    unsigned int msg11 = *(unsigned int*)((char*)&msg + 0x11);
    // +0x66：限购列表校验
    if (!G_CDataManager()->checkLimitNpcBuyItemList(msg11, (unsigned long)msgD))
    {
        // +0x82：疑似黑客用户 → trace
        int characNo = user->get_charac_no(-1);
        char* accIdStr = NumberToString(user->get_acc_id(), 0);
        cMyTrace trace(
            __PRETTY_FUNCTION__, 0x55e1, 5);
        trace("May be HackUser or Must be HackUser : m_id=%s, charac_no=%u",
              accIdStr, characNo);
        return -1;
    }

    // +0x115：交易中 → 0x13
    if (user->CheckInTrade())
        return 0x13;
    // +0x137：私商占用 → 0x3c
    if (private_store::GetInstancePrivateStoreMgr()->IsBusyPrivateStore(user))
        return 0x3c;

    // +0x163：查找物品模板
    CItem* item = G_CDataManager()->find_item((int)msgD);
    if (item == 0)
        return 0x11;

    // +0x169：所需材料查询
    Inven_Item matItem;                                           // ctor 080cb854
    std::pair<int, int>* mat = reinterpret_cast<std::pair<int, int>*>(
        NeedMaterialDataManager::getInstance()->getNeedMaterial(item));  // ORIG 0x081b5b70 返回 m_map 值指针
    if (mat->first == 0 || mat->first == -1)
        return 0x17;

    // +0x1a6：背包内材料数据
    if (((const CInventory*)user->getCurCharacInvenRefR())->GetInvenData(mat->first, matItem) < 0)
        return 0x15;
    // +0x1d8：数量校验（need < 1 或持有 < need → 0x16）
    if (mat->second < 1 || matItem.get_add_info() < mat->second)
        return 0x16;

    // +0x20b：构造购买物品（itemIdx = msg+0x0d）并尝试入包
    Inven_Item buyItem;                                           // ctor 080cb854（-0xa2）
    buyItem.m_addInfo = (int)msgD;                                // msg+0x0d → item+0x02
    G_CDataManager()->m_itemList->create_item((ENUM_ITEM_CREATE_TYPE)0, buyItem, 1);
    if (((const CInventory*)user->getCurCharacInvenRefR())->tryInsertItemIntoInventory(buyItem) < 0)
        return 0x4;
    return 0;
