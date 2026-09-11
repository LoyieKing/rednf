// [b28d-P210②] ORIG 0x0850c4ba 位于全部 caller 之后（insert_item/update_item/
// tryInsertItemSpecialSlot 均在前）⇒ 4.4 对「未见定义」的 callee 用保守 outgoing
// 帧（sub $0x18，与 ORIG 一致）；定义前置会使 caller 帧塌缩为 0xc。
void CCargo::_AddItem(const Inven_Item& item, int slot)
{
    // ORIG 为逐 dword 字段拷贝（packed 类对齐=1 路径，16×mov+movzbl 尾字节），
    // 非 memcpy 调用 ⇒ 结构体直接赋值（P54/P83/N84）。
    m_pItems[slot] = item;
    ++m_itemCount;
    m_sorted = 0;
}