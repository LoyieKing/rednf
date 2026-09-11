int CInventory::use_item(int slot, int count, int invenType)
{
    // b28c-L2 [N7 封顶 · §6.2 口径]：本函数 458/458 指令，P167 调用点差集归零；
    // 唯一残差 4 行 = 一处 CItem 虚调用的 `%edx`↔`%eax` 目标寄存器互换
    // （4.4 间接 call 目标恒 %eax，范式库 N7）。语义完成、byte 层 N 封顶。
    if (count < 0)
    {
        LogManager::logFormat(1, "inventory.cpp", __PRETTY_FUNCTION__,
                              0xf91, "count(%d) < 0", count);
        return 1;
    }
    if (invenType == 7)
    {
        if (m_pCreatureSlot[slot].m_addInfo == 0)
        {
            return 0x17;
        }
        if (m_pParent != 0)
        {
            CItem* pItem = G_CDataManager()->find_item(m_pCreatureSlot[slot].m_addInfo);
            if (pItem != 0)
            {
                int price = 0;
                G_Store()->GetSellItemPrice(m_pCreatureSlot[slot], pItem, (short)count, false,
                                            price);
                GetInstanceValueStatistic()->AddValueStatistic(
                    (VALUE_STATISTIC_FIELD)0x15, m_pParent, (unsigned int)price);
            }
        }
        if (m_pCreatureSlot[slot].m_addInfo2 == count)
        {
            int result = m_creatureMgr.UseItem(&m_pCreatureSlot[slot], slot);
            if (result != 0)
            {
                return result;
            }
            if (m_pParent != 0)
            {
                m_pParent->m_historyLog.ItemDel(
                    (INVEN_TYPE)3, 0, count, m_pCreatureSlot[slot], (eItemDelReason)3);
            }
            m_pCreatureSlot[slot].reset();
        }
        else
        {
            if (m_pCreatureSlot[slot].m_addInfo2 > count)
            {
            int result = m_creatureMgr.UseItem(&m_pCreatureSlot[slot], slot);
            if (result != 0)
            {
                return result;
            }
            m_pCreatureSlot[slot].m_addInfo2 -= count;
            if (m_pParent != 0)
            {
                m_pParent->m_historyLog.ItemDel(
                    (INVEN_TYPE)3, m_pCreatureSlot[slot].m_addInfo2, count,
                    m_pCreatureSlot[slot], (eItemDelReason)3);
            }
            }
            else
            {
                m_pCreatureSlot[slot].reset();
                LogManager::logFormat(
                    1, "inventory.cpp", __PRETTY_FUNCTION__, 0xfd5,
                    "%s - m_CreatureSlot[slot_no].add_info(%d) < count(%d)",
                    ((CUserCharacInfo*)m_pParent)->getCurCharacName(),
                    m_pCreatureSlot[slot].m_addInfo2, count);
                return 1;
            }
        }
        return 0;
    }
    if ((slot < 3 || slot > 8) && (slot < 0x39 || slot > 0x68))
    {
        return 4;
    }
    if (m_pEquipSlot[slot].m_addInfo == 0)
    {
        return 0x17;
    }
    CItem* pItem = G_CDataManager()->find_item(m_pEquipSlot[slot].m_addInfo);
    if (pItem == 0)
    {
        return 1;
    }
    if (pItem->GetItemType() == 0x12)
    {
        return 0;
    }
    if (m_pParent != 0)
    {
        int price = 0;
        G_Store()->GetSellItemPrice(m_pEquipSlot[slot], pItem, (short)count, false, price);
        GetInstanceValueStatistic()->AddValueStatistic((VALUE_STATISTIC_FIELD)0x15, m_pParent,
                                                       (unsigned int)price);
    }
    if (m_pEquipSlot[slot].m_addInfo2 == count)
    {
        int result = m_creatureMgr.UseItem(&m_pEquipSlot[slot], slot);
        if (result != 0)
        {
            return result;
        }
        if (m_pParent != 0)
        {
            m_pParent->m_historyLog.ItemDel(
                (INVEN_TYPE)1, 0, count, m_pEquipSlot[slot], (eItemDelReason)3);
        }
        m_pEquipSlot[slot].reset();
    }
    else
    {
        if (m_pEquipSlot[slot].m_addInfo2 > count)
        {
        int result = m_creatureMgr.UseItem(&m_pEquipSlot[slot], slot);
        if (result != 0)
        {
            return result;
        }
        m_pEquipSlot[slot].m_addInfo2 -= count;
        if (m_pParent != 0)
        {
            m_pParent->m_historyLog.ItemDel(
                (INVEN_TYPE)1, m_pEquipSlot[slot].m_addInfo2, count, m_pEquipSlot[slot],
                (eItemDelReason)3);
        }
        }
        else
        {
            m_pEquipSlot[slot].reset();
            LogManager::logFormat(
                1, "inventory.cpp", __PRETTY_FUNCTION__, 0x1021,
                "%s - m_InvenSlot[slot_no].add_info(%d) < count(%d)",
                ((CUserCharacInfo*)m_pParent)->getCurCharacName(),
                m_pEquipSlot[slot].m_addInfo2, count);
            return 1;
        }
    }
    return 0;
}