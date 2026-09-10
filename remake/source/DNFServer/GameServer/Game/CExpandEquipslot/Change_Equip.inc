int CExpandEquipslot::Change_Equip(CUser* pUser, int type, int slot,
                                   int space, int expandSlot)
{
    if (pUser == 0)
    {
        LogManager::logFormat(
            1, "ExpandEquipslot.cpp",
            __PRETTY_FUNCTION__,
            0xd8, "Fail Change_Equip : pUser is NULL");
        return 1;
    }
    if (pUser->get_state() < 3)
    {
        LogManager::logFormat(
            1, "ExpandEquipslot.cpp",
            __PRETTY_FUNCTION__,
            0xde, "Fail Change_Equip : pUser is not valid state");
        return 0x13;
    }
    CInventory* invenW = pUser->getCurCharacInvenW();
    switch (type)
    {
    case 0:
    {
        const Inven_Item* srcItem =
            pUser->getCurCharacInvenR()->GetInvenRef(1, slot);
        CItem* item = G_CDataManager()->find_item(srcItem->m_addInfo);
        switch (space)
        {
        case 0x12:
        {
            if (invenW->CheckValidSlot(1, slot) != 1)
            {
                return 1;
            }
            if (invenW->CheckValidSlot(5, expandSlot) != 1)
            {
                return 1;
            }
            if (pUser->getCurChannelEquipslotSwitch() == 0)
            {
                if (item != 0 &&
                    !item->IsEnableWorld((ENUM_WORLD_TYPE)1))
                {
                    return 1;
                }
            }
            Inven_Item slotItem1 = invenW->GetInvenSlot(1, slot);
            if (slotItem1.m_field0 != 0)
            {
                int secuRet = GlobalData::s_pSecuProtectionField->Check(pUser, (SECURITY_PROTCTION)0x16);
                if (secuRet != 0)
                {
                    return secuRet;
                }
            }
            if (pUser != 0)
            {
                if (pUser->CheckItemLock(1, slot))
                {
                    Inven_Item slotItem2 = invenW->GetInvenSlot(1, slot);
                    if (slotItem2.m_field0 != 0)
                    {
                        return 0xd5;
                    }
                }
            }
            int moveRet = 0;
            // b28d-65 真差异修正：ORIG 0x08499cb0 段压栈为 (pUser,type,slot,space,expandSlot-10)
            // —— 实参来源是形参 type/space（分支保证其值为 0/0x12），非字面量（P97/P171 照抄来源）。
            moveRet = Move_Item(pUser, type, slot, space, expandSlot - 10);
            if (moveRet != 0)
            {
                return moveRet;
            }
            alter();
            return 0;
        }
        default:
            return 1;
        }
    }
    default:
        return 1;
    }
