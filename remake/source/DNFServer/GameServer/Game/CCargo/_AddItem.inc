void CCargo::_AddItem(const Inven_Item& item, int slot)
{
    // ORIG 为逐 dword 字段拷贝（packed 类对齐=1 路径，16×mov+movzbl 尾字节），
    // 非 memcpy 调用 ⇒ 结构体直接赋值（P54/P83/N84）。
    m_pItems[slot] = item;
    ++m_itemCount;
    m_sorted = 0;
