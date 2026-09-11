void CInventory::SetInvenData(int invenType, void* in, int size)
{
    switch (invenType)
    {
    case 0:
        if (size == 0x632)
        {
            memcpy(m_invenItem, in, size);
        }
        break;
    case 1:
        if (size == 0x4a58)
        {
            memcpy(m_pEquipSlot, in, size);
        }
        break;
    case 2:
        if (size == 0x1905)
        {
            memcpy(m_pAvatarSlot, in, size);
        }
        break;
    case 3:
        if (size == 0x39aa)
        {
            memcpy(m_pCreatureSlot, in, size);
        }
        break;
    case 5:
        if (size == 0x2dc)
        {
            memcpy(&m_invenItem[10], in, size);
        }
        break;
    }
}