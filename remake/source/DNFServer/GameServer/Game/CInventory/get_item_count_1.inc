int CInventory::get_item_count(int itemIdx, unsigned long param) const
{
    int count = 0;
    // ORIG 0x84fb50d..+0x35: 4.1 switch 下探形（与 get_item_count(int) 同构）；
    // case1 命中体二次装载 0x7 字段全字 add（0x84fb595..+0xaa）。
    switch (itemIdx)
    {
    case 0:
        for (int i = 0; i < 0x16; i++)
        {
            if ((unsigned long)m_invenItem[i].m_addInfo == param)
            {
                count++;
            }
        }
        break;
    case 1:
        for (int i = 3; i < 0x138; i++)
        {
            if ((unsigned long)m_pEquipSlot[i].m_addInfo == param)
            {
                count += m_pEquipSlot[i].m_addInfo2;
            }
        }
        break;
    case 2:
        for (int i = 0; i < 0x69; i++)
        {
            if ((unsigned long)m_pAvatarSlot[i].m_addInfo == param)
            {
                count++;
            }
        }
        break;
    case 3:
        for (int i = 0; i < 0xf2; i++)
        {
            if ((unsigned long)m_pCreatureSlot[i].m_addInfo == param)
            {
                count++;
            }
        }
        break;
    default:
        break;
    }
    return count;
