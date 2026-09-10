int CInventory::getAvatarEmptySlot(int itemIdx, int param) const
{
    for (int i = itemIdx; i <= param; i++)
    {
        if (m_pAvatarSlot[i].m_addInfo == 0)
        {
            return i;
        }
    }
    return -1;
