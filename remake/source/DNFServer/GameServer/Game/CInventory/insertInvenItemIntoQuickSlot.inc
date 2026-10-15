int CInventory::insertInvenItemIntoQuickSlot(Inven_Item item, eItemAddReason reason, bool bLog)
{
    for (int i = 3; i <= 8; i++)
    {
        if (m_pEquipSlot[i].m_addInfo == 0)
        {
            if (item.m_itemType != 1)
            {
                if (!checkStackableLimit((unsigned long)item.m_addInfo, item.m_addInfo2))
                {
                    return -10;
                }
            }
            m_pEquipSlot[i].setCopy(item);
            if (bLog)
            {
                if (m_pParent != 0)
                {
                    if (item.m_itemType == 1)
                    {
                        m_pParent->m_historyLog.ItemAdd(
                            (INVEN_TYPE)1, 1, 1, item, reason);
                    }
                    else
                    {
                        m_pParent->m_historyLog.ItemAdd(
                            (INVEN_TYPE)1, m_pEquipSlot[i].get_add_info(), item.get_add_info(),
                            item, reason);
                    }
                }
                else
                {
                    LogManager::logFormat(
                        1, "inventory.cpp",
                        __PRETTY_FUNCTION__,
                        0x1528, "History ERROR, m_pParent NULL, ItemAdd ");
                }
            }
            return i;
        }
    }
    return -2;
}