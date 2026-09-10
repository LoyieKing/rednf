int CInventory::insertAvatarIntoSpecificSlot(Inven_Item item, int slot,
                                              eAvatarItemAddReason reason, bool bLog)
{
    if (slot >= 0x69)
    {
        return -1;
    }
    int result = -1;
    if (m_pAvatarSlot[slot].m_addInfo == 0)
    {
        m_pAvatarSlot[slot] = item;
        result = slot;
    }
    else
    {
        for (int i = 0; i < 0x69; i++)
        {
            if (m_pAvatarSlot[i].m_addInfo == 0)
            {
                m_pAvatarSlot[i] = item;
                result = i;
                break;
            }
        }
    }
    if (result >= 0 && bLog)
    {
        const char* agency = m_avatarItemMgr.GetIPGAgencyNo(
            m_pAvatarSlot[result].get_add_info());
        m_pParent->m_historyLog.AvatarItemAdd(
            item.m_addInfo, m_pAvatarSlot[result].get_add_info(),
            agency == 0 ? "" : agency, reason);
    }
    return result;
