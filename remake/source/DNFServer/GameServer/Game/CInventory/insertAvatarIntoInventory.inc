int CInventory::insertAvatarIntoInventory(Inven_Item item, eAvatarItemAddReason reason, bool bLog)
{
    for (int i = 0; i <= 0x68; i++)
    {
        if (m_pAvatarSlot[i].m_addInfo == 0)
        {
            m_pAvatarSlot[i] = item;
            if (bLog)
            {
                const char* agency = m_avatarItemMgr.GetIPGAgencyNo(
                    m_pAvatarSlot[i].get_add_info());
                m_pParent->m_historyLog.AvatarItemAdd(
                    item.m_addInfo, m_pAvatarSlot[i].get_add_info(),
                    agency == 0 ? "" : agency, reason);
            }
            return i;
        }
    }
    return -1;
}