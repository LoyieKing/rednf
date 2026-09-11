// ---- CopyItems（ORIG T 0x85c4f9c，报告 CopyItems.md）----
// 先 ResetItems，再把 items[0..count) 拷到 m_items 头部（std::copy，步长 0x4c）。
void CPrivateStore::CopyItems(PrivateStoreItem* items, int count)
{
    ResetItems();
    std::copy(items, items + count, m_items.begin());
}