int CInventory::check_vilmark_dungeon_item_exist(int itemIdx) const
{
    // ORIG 0x8505530：i(-0xc)/start(-0x10)/end(-0x14) 单槽复用、无初始化
    // dead store（0x850556c 直接 lea）。声明序=近槽优先（技巧21 实测反向）。
    int i;
    int start;
    int end;
    for (i = 3; i <= 8; i++)
    {
        if (m_pEquipSlot[i].m_addInfo == itemIdx)
        {
            return i;
        }
    }
    StartEndPos((Inven_Item::ITEM_TYPE)3, start, end);
    for (i = start; i <= end; i++)
    {
        if (m_pEquipSlot[i].m_addInfo == itemIdx)
        {
            return i;
        }
    }
    StartEndPos((Inven_Item::ITEM_TYPE)4, start, end);
    for (i = start; i <= end; i++)
    {
        if (m_pEquipSlot[i].m_addInfo == itemIdx)
        {
            return i;
        }
    }
    return -1;
}