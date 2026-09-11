int CInventory::getExistEquipItem(int itemIdx, int count, int slot)
{
    int nField = 0;  // ORIG 顶部有未使用局部（-0x8(%ebp) 初始化为 0）
    // b28c-L2: ORIG 0x850348b..0x8503494 四个失败目标各落 nop 岛 + `jmp <addl>`
    // （0x850348d/490/493 三条）⇒ 负条件 goto 尾标签形（探针 l2_probe7 P 命中）。
    for (int i = 3; i <= 0xc8; i++)
    {
        if (i == slot) goto next_slot;
        if (m_pEquipSlot[i].m_field1 != 1) goto next_slot;
        if (m_pEquipSlot[i].m_addInfo != itemIdx) goto next_slot;
        if (m_pEquipSlot[i].m_addInfo2 != count) goto next_slot;
        return i;
    next_slot:;
    }
    return -1;
}