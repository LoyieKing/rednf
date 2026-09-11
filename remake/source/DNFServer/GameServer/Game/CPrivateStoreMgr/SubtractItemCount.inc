// ---- SubtractItemCount（ORIG T 0x85c7bbc，报告 SubtractItemCount.md 全文）----
// 计数字段 = 条目 +0x07 = m_item.m_addInfo2。非堆叠或减至恰好相等时整条 Reset。
void CPrivateStore::SubtractItemCount(CItem const* item, PrivateStoreItem& storeItem,
                                      int count)
{
    // ORIG 0x085c7bbc：正向 `if (is_stackable())`（test%al;je 直判），
    // !stackable 的 Reset 位于尾部跳岛（+0x40），非 inline。
    if (item->is_stackable())
    {
        if ((int)storeItem.m_item.m_addInfo2 == count)
            storeItem.Reset();
        else
            storeItem.m_item.m_addInfo2 -= count;
    }
    else
    {
        storeItem.Reset();
    }
}