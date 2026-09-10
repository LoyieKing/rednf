int CInventory::try_use_item(int slot, int count, int invenType) const
{
    // b28c-L2 [N7 封顶]：唯一虚调用 pItem->GetItemType()（ORIG 0x0850… 区间）为
    // `mov (%eax),%edx; mov -0xc(%ebp),%eax; mov %eax,(%esp); call *%edx`，4.4-O0
    // 对间接 call 目标恒 %eax（N7，附 4.1 按上下文选 edx/ecx）⇒ 4 行寄存器名差封顶。
    if (count < 0)
    {
        return 1;
    }
    if (invenType == 7)
    {
        return 1;
    }
    if ((slot < 3 || slot > 8) && (slot < 0x39 || slot > 0x68))
    {
        return 4;
    }
    if (m_pEquipSlot[slot].m_addInfo == 0)
    {
        return 0x17;
    }
    CItem* pItem = G_CDataManager()->find_item(m_pEquipSlot[slot].m_addInfo);
    if (pItem == 0)
    {
        return 1;
    }
    if (pItem->GetItemType() == 0x12)
    {
        return 0;
    }
    if (m_pEquipSlot[slot].m_addInfo2 < count)
    {
        return 1;
    }
    return 0;
