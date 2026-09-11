int CInventory::MoveItemToEmptySlotOfInventory(int slot, int start, int end)
{
    // b28c-L2: ORIG 0x85049c8 形——入口 `jmp cond` + 尾 `cmp/setle/test/jne`
    // 即 while(i<=end)（P25/P51）；item 的 Inven_Item C1 call 落在循环之后
    // （0x8504a22），故声明置于早退检查之后。
    int dst = -1;
    int i = start;
    while (i <= end)
    {
        if (m_pEquipSlot[i].m_addInfo == 0)
        {
            dst = i;
            break;
        }
        i++;
    }
    if (dst == -1)
    {
        return 0x16;
    }
    Inven_Item item;
    item.setCopy(m_pEquipSlot[slot]);
    m_pEquipSlot[slot].reset();
    m_pEquipSlot[dst].setCopy(item);
    return 0;
}