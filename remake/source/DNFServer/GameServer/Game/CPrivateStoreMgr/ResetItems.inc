// ---- ResetItems（ORIG T 0x85c4f52，报告 private_store__CPrivateStore/ResetItems.md）----
// 逐项调用 PrivateStoreItem::Reset（m_items 恒为 14 项，ctor resize(0xe)）。
void CPrivateStore::ResetItems()
{
    for (int i = 0; i < (int)m_items.size(); ++i)
        m_items[i].Reset();
}