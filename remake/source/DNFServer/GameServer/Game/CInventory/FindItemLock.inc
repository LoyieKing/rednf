bool CInventory::FindItemLock(unsigned char b, ENUM_ITEMSPACE& space, int& slot) const
{
    space = (ENUM_ITEMSPACE)3;
    slot = 0;
    while (slot < 0x16)
    {
        // ORIG 0x84fac9e-0x84faca4：GetLock(this+i*0x3d+0x2d)，+0x2d=this+0x1c+0x11
        // = m_invenItem[i].m_amp（FindItemLock.md）。
        if (m_invenItem[slot].m_amp.GetLock() == b)
        {
            return 1;
        }
        slot++;
    }
    space = (ENUM_ITEMSPACE)0;
    slot = 0;
    while (slot < 0x138)
    {
        if (m_pEquipSlot[slot].m_amp.GetLock() == b)
        {
            return 1;
        }
        slot++;
    }
    space = (ENUM_ITEMSPACE)7;
    slot = 0;
    while (slot < 0xf2)
    {
        if (m_pCreatureSlot[slot].m_amp.GetLock() == b)
        {
            return 1;
        }
        slot++;
    }
    space = (ENUM_ITEMSPACE)1;
    slot = 0;
    while (slot <= 0x68)
    {
        if (m_pAvatarSlot[slot].m_amp.GetLock() == b)
        {
            return 1;
        }
        slot++;
    }
    return 0;
