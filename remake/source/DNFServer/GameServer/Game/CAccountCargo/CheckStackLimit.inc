bool CAccountCargo::CheckStackLimit(int slot, int itemIdx, int count)
{
    // [b28d-solo-CAccountCargo] ORIG 0x0828a670：G_CDataManager 结果无槽往返
    // （P0 内联实参，禁 dm 具名局部）；ci 槽 -0x14 → si 槽 -0x10 → sum 槽 -0xc
    // 声明序（P28）；条件为 `limit < sum || sum < 0`（P133 短路链）。
    if (!CheckValidSlot(slot))
        return 0;
    if (m_slots[slot].m_addInfo != itemIdx)
        return 0;
    CItem* ci = G_CDataManager()->find_item(itemIdx);
    if (ci == 0)
        return 0;
    if (!ci->is_stackable())
        return 0;
    CStackableItem* si = (CStackableItem*)ci;
    int sum = m_slots[slot].m_addInfo2 + count;
    if (si->getStackableLimit() < sum || sum < 0)
        return 0;
    return 1;
}