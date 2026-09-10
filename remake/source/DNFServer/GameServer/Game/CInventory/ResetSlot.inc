void CInventory::ResetSlot(int invenType, int slot)
{
    // ORIG 0x84fb32a：switch(invenType) 形态（一次载入寄存器后 cmp/je 二叉判定树）。
    if (CheckValidSlot(invenType, slot) != 1)
    {
        return;
    }
    switch (invenType)
    {
    case 0:
        m_invenItem[slot].reset();
        break;
    case 1:
        m_pEquipSlot[slot].reset();
        break;
    case 2:
        m_pAvatarSlot[slot].reset();
        break;
    case 3:
        m_pCreatureSlot[slot].reset();
        break;
    }
