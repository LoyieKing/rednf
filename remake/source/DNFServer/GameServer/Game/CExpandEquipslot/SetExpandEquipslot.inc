void CExpandEquipslot::SetExpandEquipslot(int type, void* data, int size)
{
    switch (type)
    {
    case 1:
        if (size == 0x2dc)
        {
            memcpy(m_equipSlot, data, size);
            alter();
        }
        break;
    case 2:
        if (size == 0x2dc)
        {
            memcpy(m_expandEquipSlot, data, size);
            alter();
        }
        break;
    case 3:
        if (size == 0x2dc)
        {
            memcpy(m_expandAvatarSlot, data, size);
            alter();
        }
        break;
    default:
        break;
    }
}