bool CAccountCargo::DeleteItem(int slot, int count)
{
    // [b28d-solo-CAccountCargo] ORIG 0x08289e3c：两个独立早返（P133 拆守卫，
    // 各自 `mov $0; jmp` 出口）+ `isEquipableItemType` 正极性（ORIG `test %al;
    // je <非装备>`，无 xor ⇒ 禁 ! 形）。
    if (!CheckValidSlot(slot))
        return 0;
    if (m_slots[slot].m_addInfo == 0 || count < 1)
        return 0;
    if (m_slots[slot].isEquipableItemType()) {
        ResetSlot(slot);
    } else {
        if (m_slots[slot].m_addInfo2 < count)
            return 0;
        if (count < m_slots[slot].m_addInfo2)
            m_slots[slot].m_addInfo2 = m_slots[slot].m_addInfo2 - count;
        else
            ResetSlot(slot);
    }
    m_alter = 1;
    return 1;
