int CAccountCargo::InsertItem(Inven_Item& item, int slot)
{
    // [b28d-solo-CAccountCargo 真语义 bug] ORIG 0x08289c82：非装备路径为
    // 「同类累加 / 异类直接结构覆盖」，**不存在 return -1 分支**（原源码多写
    // -1 路径=行为偏差，Ghidra C 与反汇编双向证实）。ret 具名局部 -0xc（P0 有槽
    // 往返），isEquipable 正极性（`test %al; je <非装备块>`，装备块 fall-through）。
    if (!CheckValidSlot(slot))
        return -1;
    int ret = -1;
    if (item.isEquipableItemType()) {
        if (m_slots[slot].m_addInfo == 0) {
            m_slots[slot] = item;
            ret = slot;
        }
    } else {
        if (item.m_addInfo == m_slots[slot].m_addInfo) {
            m_slots[slot].m_addInfo2 = m_slots[slot].m_addInfo2 + item.m_addInfo2;
            ret = slot;
        } else {
            m_slots[slot] = item;
            ret = slot;
        }
    }
    m_alter = 1;
    return ret;
