int CInventory::doesExistEquipItem(int itemIdx, int slot)
{
    // b28c-L2: ORIG 0x85033f8/fb/fe 三失败目标 + 0x85033ff/402 两条 `jmp <addl>`
    // nop 岛 ⇒ 负条件 goto 尾标签形（探针 l2_probe7 Q 命中）。
    for (int i = 3; i <= 0xc8; i++)
    {
        if (m_pEquipSlot[i].m_addInfo != itemIdx) goto next_slot;
        if (m_pEquipSlot[i].m_addInfo2 != slot) goto next_slot;
        if (m_pEquipSlot[i].m_field1 != 1) goto next_slot;
        return 1;
    next_slot:;
    }
    return 0;
}