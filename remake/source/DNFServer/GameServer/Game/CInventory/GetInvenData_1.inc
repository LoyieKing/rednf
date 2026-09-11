int CInventory::GetInvenData(int itemIdx, Inven_Item& out) const
{
    // b28d-Inventory2 本轮：214/213 DIFF → 214/214 AE。两处真因：①type 局部声明为
    // Inven_Item::ITEM_TYPE（enum）⇒ 复现 ORIG `lea -0x20,%eax … lea -0x1c,%eax …
    // mov -0x18,%eax` 的 %eax 串行装载（int 局部时 4.4 提前载 type 并使 lea 走 %edx）；
    // ②creature 分支循环后显式 return -1 ⇒ 复现 ORIG 0x84fc041 内联
    // `mov $0xffffffff,%eax; jmp <尾>`（共享尾形丢该 store，P63）。
    Inven_Item::ITEM_TYPE type = (Inven_Item::ITEM_TYPE)GetItemType(itemIdx);
    int start;
    int end;
    if (type == 0)
    {
        return -1;
    }
    StartEndPos((Inven_Item::ITEM_TYPE)type, start, end);
    if (IsCreatureItemType(type))
    {
        for (int i = start; i <= end; i++)
        {
            if (m_pCreatureSlot[i].m_addInfo == itemIdx)
            {
                out = m_pCreatureSlot[i];
                return i;
            }
        }
        // b28d-Inventory2：ORIG 0x84fc041 内联 `mov $0xffffffff,%eax; jmp <尾>`，
        // 共享尾形会丢该 store（P63/P223 族）⇒ 分支内显式 return。
        return -1;
    }
    else
    {
        for (int i = start; i <= end; i++)
        {
            if (m_pEquipSlot[i].m_addInfo == itemIdx)
            {
                out = m_pEquipSlot[i];
                return i;
            }
        }
        StartEndPos(Inven_Item::ITEM_TYPE_0, start, end);
        for (int i = start; i <= end; i++)
        {
            if (m_pEquipSlot[i].m_addInfo == itemIdx)
            {
                out = m_pEquipSlot[i];
                return i;
            }
        }
    }
    return -1;
}