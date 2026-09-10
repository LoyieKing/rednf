int CInventory::insertCreatureIntoSpecificSlot(Inven_Item item, int slot,
                                                eItemAddReason reason, bool bLog)
{
    if (slot >= 0xf2)
    {
        return -1;
    }
    if (item.m_addInfo == 0)
    {
        return -1;
    }
    int result = -1;
    if (m_pCreatureSlot[slot].m_addInfo == 0)
    {
        m_pCreatureSlot[slot] = item;
        result = slot;
    }
    else if (!m_pCreatureSlot[slot].isEquipableItemType())
    {
        if (m_pCreatureSlot[slot].m_addInfo == item.m_addInfo)
        {
            if (!checkStackableLimit((unsigned long)m_pCreatureSlot[slot].m_addInfo,
                                     m_pCreatureSlot[slot].get_add_info() + item.get_add_info()))
            {
                return -10;
            }
            m_pCreatureSlot[slot].set_add_info(
                m_pCreatureSlot[slot].get_add_info() + item.get_add_info());
            result = slot;
        }
        else
        {
            for (int i = 0; i < 0xf2; i++)
            {
                if (m_pCreatureSlot[i].m_addInfo == 0)
                {
                    m_pCreatureSlot[i] = item;
                    result = i;
                    break;
                }
                if (m_pCreatureSlot[i].m_addInfo == item.m_addInfo)
                {
                    m_pCreatureSlot[i].set_add_info(
                        m_pCreatureSlot[i].get_add_info() + item.get_add_info());
                    result = i;
                    break;
                }
            }
        }
    }
    if (result >= 0 && item.m_field1 != 5 && bLog)
    {
        m_pParent->m_historyLog.CreatureItemAdd(
            (INVEN_TYPE)3, item.m_addInfo, m_pCreatureSlot[result].get_add_info(),
            item.get_add_info(), item.m_field1, reason);
    }
    return result;
