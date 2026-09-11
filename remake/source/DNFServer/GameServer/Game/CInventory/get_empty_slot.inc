int CInventory::get_empty_slot(INVEN_TYPE invenType, Inven_Item::ITEM_TYPE type) const
{
    // ORIG 0x84fb82a..+0x1a: switch 下探（mov 0xc,%eax 缓存；cmp2 je→+0x7c；
    // cmp3 je→+0xb4；cmp1 jne→+0xed），体排布 case1/case2/case3（技巧34）。
    switch (invenType)
    {
    case 1:
    {
        // ORIG 0x84fb844: start=-0x18 end=-0x1c 无初始化 dead store；i=-0x14。
        int start;
        int end;
        StartEndPos(type, start, end);
        for (int i = start; i <= end; i++)
        {
            if (m_pEquipSlot[i].m_addInfo == 0)
            {
                return i;
            }
        }
        break;
    }
    case 2:
        for (int i = 0; i < 0x69; i++)
        {
            if (m_pAvatarSlot[i].m_addInfo == 0)
            {
                return i;
            }
        }
        break;
    case 3:
        for (int i = 0; i < 0xf2; i++)
        {
            if (m_pCreatureSlot[i].m_addInfo == 0)
            {
                return i;
            }
        }
        break;
    default:
        break;
    }
    return -1;
}