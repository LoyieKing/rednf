// ---- Clear（ORIG T 0x85c4e70，报告 Clear.md）----
// 清卖家/买家指针、状态、位置、围观列表并 ResetItems；随后清 doll 标志与 doll 物品。
// ORIG 不清 m_pParent 与店名。
void CPrivateStore::Clear()
{
    m_pSeller = 0;
    m_pBuyer = 0;
    m_state = 0;
    m_pos.clear();
    m_users.clear();
    ResetItems();
    m_dollCreated = 0;
    m_dollItemID = 0;
}