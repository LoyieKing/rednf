int CInventory::get_item_count(int itemIdx) const
{
    int count = 0;
    // ORIG 0x84fb3f1..+0x31: 4.1 switch 二叉下探（先 mov 0xc,%eax 缓存，
    // cmp $1 je case1；cmp $1 jg→{2,3}；test je case0），case 体按 0/1/2/3
    // 顺序排布（技巧 34）。i 槽 case0=-0x10 case1=-0xc case2=-0x8 case3=-0x4。
    switch (itemIdx)
    {
    case 0:
        for (int i = 0; i < 0x16; i++)
        {
            if (m_invenItem[i].m_addInfo != 0)
            {
                count++;
            }
        }
        break;
    case 1:
        for (int i = 3; i < 0x138; i++)
        {
            if (m_pEquipSlot[i].m_addInfo != 0)
            {
                count++;
            }
        }
        break;
    case 2:
        for (int i = 0; i < 0x69; i++)
        {
            if (m_pAvatarSlot[i].m_addInfo != 0)
            {
                count++;
            }
        }
        break;
    case 3:
        for (int i = 0; i < 0xf2; i++)
        {
            if (m_pCreatureSlot[i].m_addInfo != 0)
            {
                count++;
            }
        }
        break;
    default:
        break;
    }
    return count;
