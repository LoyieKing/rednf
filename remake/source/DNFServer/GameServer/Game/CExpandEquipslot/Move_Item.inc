int CExpandEquipslot::Move_Item(CUser* pUser, int type, int slot, int space,
                                int expandSlot)
{
    if (pUser == 0)
    {
        LogManager::logFormat(
            1, "ExpandEquipslot.cpp",
            __PRETTY_FUNCTION__,
            0x17c, "Fail Move_Item : pUser is NULL");
        return 0x11;
    }
    if (pUser->get_state() < 3)
    {
        LogManager::logFormat(
            1, "ExpandEquipslot.cpp",
            __PRETTY_FUNCTION__,
            0x182, "Fail Move_Item : pUser is not valid state");
        return 0x11;
    }
    CInventory* inven = pUser->getCurCharacInvenW();
    switch (type)
    {
    case 0:
        switch (space)
        {
        case 0x12:
        {
            if (!CheckEquipable(
                    inven->m_pEquipSlot[slot],
                    m_equipSlot[expandSlot]))
            {
                return 0x11;
            }
            if (!inven->m_pEquipSlot[slot].m_amp.isIdentified() ||
                !m_equipSlot[expandSlot].m_amp.isIdentified())
            {
                return 0x11;
            }
            Inven_Item localItem;
            localItem.setCopy(inven->m_pEquipSlot[slot]);
            if (localItem.m_addInfo != 0)
            {
                CItem* item =
                    G_CDataManager()->find_item(localItem.m_addInfo);
                if (item == 0 || pUser == 0)
                {
                    cMyTrace(
                        __PRETTY_FUNCTION__,
                        0x1a0, 5)(
                        "CExpandEquipslot::Move_Item Move_Item return false (%d)",
                        localItem.m_addInfo);
                    return 0x11;
                }
                if (item->IsAvatarItem())
                {
                    return 0x11;
                }
                if (item->IsCreatureItem())
                {
                    return 0x11;
                }
                int nEquip = 0;
                nEquip = IsEquipable(pUser, item, expandSlot + 10);
                if (nEquip != 0)
                {
                    return nEquip;
                }
                if (item->PeriodAfterUnsealing() != 0)
                {
                    int nExpire = GlobalData::s_systemTime_.getCurSec() +
                                  item->PeriodAfterUnsealing() * 0x15180;
                    if (nExpire != 0)
                    {
                        localItem.m_fieldb =
                            (unsigned short)((nExpire - 0x44a54a80) /
                                             0x15180);
                    }
                }
            }
            inven->m_pEquipSlot[slot].setCopy(
                m_equipSlot[expandSlot]);
            localItem.m_field0 = 0;
            m_equipSlot[expandSlot].setCopy(localItem);
            return 0;
        }
        default:
            return 0x11;
        }
    default:
        return 0x11;
    }
