int CInventory::insertItemIntoCreatureStackable(Inven_Item item, eItemAddReason reason, bool bLog)
{
    if (item.m_addInfo == 0)
    {
        return -1;
    }
    int slot = GetExistCreatureSlot(item);
    if (slot >= 0)
    {
        if (0x7fffffff - item.get_add_info() <= m_pCreatureSlot[slot].get_add_info())
        {
        }
        else
        {
            if (!checkStackableLimit((unsigned long)m_pCreatureSlot[slot].m_addInfo,
                                     m_pCreatureSlot[slot].get_add_info() + item.get_add_info()))
            {
                return -10;
            }
            m_pCreatureSlot[slot].set_add_info(
                m_pCreatureSlot[slot].get_add_info() + item.get_add_info());
            if (bLog)
            {
                WriteCreatureLog((INVEN_TYPE)1, item.m_addInfo, m_pCreatureSlot[slot].get_add_info(),
                                 item.get_add_info(), item.m_itemType, reason);
            }
            return slot;
        }
    }
    slot = GetEmptyCreatureSlot(item);
    if (slot < 0)
    {
        return -1;
    }
    m_pCreatureSlot[slot].setCopy(item);
    if (bLog)
    {
        WriteCreatureLog((INVEN_TYPE)3, item.m_addInfo, m_pCreatureSlot[slot].get_add_info(),
                         item.get_add_info(), item.m_itemType, reason);
    }
    return slot;
}