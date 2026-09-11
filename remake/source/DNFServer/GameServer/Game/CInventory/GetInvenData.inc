void CInventory::GetInvenData(int invenType, void* out, int size) const
{
    // b28d-solo-CInventory：已修——5 处 memcpy 长度改用形参 size（ORIG 实证 0x14(%ebp)），73/73 仅余重定位伪差 ⇒ AE（待 relink）。
    switch (invenType)
    {
    case 0:
        if (size == 0x632)
        {
            memcpy(out, m_invenItem, size);
        }
        break;
    case 1:
        if (size == 0x4a58)
        {
            memcpy(out, m_pEquipSlot, size);
        }
        break;
    case 2:
        if (size == 0x1905)
        {
            memcpy(out, m_pAvatarSlot, size);
        }
        break;
    case 3:
        if (size == 0x39aa)
        {
            memcpy(out, m_pCreatureSlot, size);
        }
        break;
    case 5:
        if (size == 0x2dc)
        {
            memcpy(out, &m_invenItem[10], size);
        }
        break;
    }
}