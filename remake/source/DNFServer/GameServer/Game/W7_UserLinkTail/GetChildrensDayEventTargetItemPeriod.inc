// ---------------------------------------------------------------------------
// CUser::GetChildrensDayEventTargetItemPeriod(int idx) W 0x08328262
// idx>9 → -1；否则读 periods 表 @0x8e0c4 + idx*4。
// ---------------------------------------------------------------------------
int CUser::GetChildrensDayEventTargetItemPeriod(int idx)
{
    if (idx > 9)
        return -1;
    return m_childrensDayPeriods[idx];
}
