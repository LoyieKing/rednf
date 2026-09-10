int CInventory::tryStackInvenItemIntoQuickSlot(Inven_Item item) const
{
    if (item.m_field1 == 1)
    {
        return -1;
    }
    for (int i = 3; i <= 8; i++)
    {
        if (m_pEquipSlot[i].m_addInfo != item.m_addInfo)
        {
            continue;
        }
        if (0x7fffffff - item.get_add_info() <= m_pEquipSlot[i].get_add_info())
        {
            return -2;
        }
        // b28d-37：ORIG 0x08501ca5..0xb2 对和值有槽往返（`lea (%ebx,%eax,1),%eax;
        // mov %eax,-0xc(%ebp); mov -0xc(%ebp),%edx`）⇒ P0 原码有具名局部；
        // 直传形少 2 条且循环变量槽上移 4（-0xc vs ORIG -0x10）。
        int sum = m_pEquipSlot[i].get_add_info() + item.get_add_info();
        if (!checkStackableLimit((unsigned long)item.m_addInfo, sum))
        {
            return -10;
        }
        return i;
    }
    return -4;
