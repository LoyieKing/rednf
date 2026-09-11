// ---------------------------------------------------------------------------
// CUser::GetChildrensDayEventTargetItemSlot(int idx) W 0x08328240
// idx>9 → -1；否则读 slots 表 @0x8e09c + idx*4。
// ---------------------------------------------------------------------------
int CUser::GetChildrensDayEventTargetItemSlot(int idx)
{
    if (idx > 9)
        return -1;
    return m_childrensDaySlots[idx];           // P30 [i+K] 两步寻址源形
}
