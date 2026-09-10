int CInventory::get_avatar_slot_no(int avatarUid) const
{
    for (int i = 0; i <= 0x68; i++)
    {
        if (m_pAvatarSlot[i].m_addInfo == avatarUid)
        {
            return i;
        }
    }
    return -1;
