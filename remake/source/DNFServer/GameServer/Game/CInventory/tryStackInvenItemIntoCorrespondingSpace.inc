int CInventory::tryStackInvenItemIntoCorrespondingSpace(Inven_Item item) const
{
    // b28d-37 [已修复 AE，ORIG 0x08501cea]：P0 具名 sum 局部（ORIG 0x08501dca..0xe3 槽往返）复现 87→89=ORIG；fresh 复验零差（仅 .o 重定位伪差）。
    if (item.m_field1 == 1)
    {
        return -1;
    }
    int start = 0;
    int end = 0;
    getStartEndPos((Inven_Item::ITEM_TYPE)item.m_field1, start, end);
    for (int i = start; i <= end; i++)
    {
        if (m_pEquipSlot[i].m_addInfo != item.m_addInfo)
        {
            continue;
        }
        if (0x7fffffff - item.get_add_info() <= m_pEquipSlot[i].get_add_info())
        {
            return -2;
        }
        // b28d-37：同 tryStackInvenItemIntoQuickSlot——ORIG 0x08501dca..0xe3 有
        // `mov %eax,-0xc(%ebp); mov -0xc(%ebp),%edx` 槽往返 ⇒ P0 具名 sum 局部。
        int sum = m_pEquipSlot[i].get_add_info() + item.get_add_info();
        if (!checkStackableLimit((unsigned long)item.m_addInfo, sum))
        {
            return -10;
        }
        return i;
    }
    return -4;
