int CExpandEquipslot::EquipslotSwitch(CUser* pUser, char flag,
                                      char channelSwitch, char expandSwitch)
{
    if (pUser == 0)
    {
        LogManager::logFormat(
            1, "ExpandEquipslot.cpp",
            __PRETTY_FUNCTION__,
            0x44, "Fail Equipslot Switch : pUser is NULL");
        return 1;
    }
    if (pUser->get_state() < 3)
    {
        LogManager::logFormat(
            1, "ExpandEquipslot.cpp",
            __PRETTY_FUNCTION__,
            0x4a, "Fail Equipslot Switch : pUser is not valid state");
        return 0x13;
    }
    char curChannel = pUser->getCurChannelEquipslotSwitch();
    char curExpand = pUser->getCurExpandEquipslotSwitch();
    if (curChannel != channelSwitch || curExpand != expandSwitch)
    {
        return 1;
    }
    item_lock::CItemLock* lock =
        (item_lock::CItemLock*)pUser->GetCharacExpandData(
            (ENUM_CHARAC_EXPAND_TYPE)2);
    std::vector<item_lock::stItemLockRef>& refVec =
        lock->GetItemLockRefVec();
    for (std::vector<item_lock::stItemLockRef>::iterator it = refVec.begin();
         it != refVec.end(); it++)
    {
        item_lock::stItemLockRef& ref = *it;
        // b28d-65：ORIG 0x084995b8 段用 jbe/ja（无符号）⇒ m_slot(short) 按无符号比较。
        if ((unsigned short)ref.m_slot > 9 && (unsigned short)ref.m_slot < 0x16)
        {
            if (ref.m_space == 3)
            {
                ref.m_space = 0x12;
            }
            else if (ref.m_space == 0x12)
            {
                ref.m_space = 3;
            }
        }
    }
    if (flag == 0)
    {
        CInventory* inven = pUser->getCurCharacInvenW();
        Inven_Item expandInven[12];
        Inven_Item saveExpand;
        Inven_Item saveEquip;
        Inven_Item slotItem1 = inven->GetInvenSlot(0, 11);
        saveEquip.setCopy(slotItem1);
        saveExpand.setCopy(m_equipSlot[1]);
        inven->GetInvenData(5, expandInven, 0x2dc);
        inven->SetInvenData(5, m_equipSlot, 0x2dc);
        SetExpandEquipslot(1, expandInven, 0x2dc);
        inven->m_invenItem[11].setCopy(saveEquip);
        m_equipSlot[1].setCopy(saveExpand);
        CParty* party = pUser->GetParty();
        if (party != 0)
        {
            party->Reset_party_overlapped_drop_ratio();
        }
        Inven_Item slotItem2 = inven->GetInvenSlot(0, 10);
        // b28d-65 [N/P214①]：ORIG 0x084997af 段对 `a!=0||b!=0` 物化 `mov$1/mov$0`+test%al
        // （4.1 短路汇聚物化）；4.4-O0 恒折直跳。具名 bool 可补物化岛但落槽 2 条
        // 且把 curChannel 槽 -0x1e 顶到 -0x22（P226 零槽形语句表达式实测无效）⇒ 保持直跳形。
        if (slotItem2.m_addInfo != 0 || m_equipSlot[0].m_addInfo != 0)
        {
            // CUser 冻结头 +0x796f4 即 CCharacterView（本 TU 用本地最小 CUser 视图，
            // 不能并入 CUser.h；保持具名视图 + 文档化基址访问）
            ((CCharacterView*)((char*)pUser + 0x796f4))
                ->enableSaveCharacView();
            pUser->send_equip(10);
        }
        if (curChannel == 0)
        {
            pUser->setCurChannelEquipslotSwitch(1);
        }
        else if (curChannel == 1)
        {
            pUser->setCurChannelEquipslotSwitch(0);
        }
        return 0;
    }
    return 1;
