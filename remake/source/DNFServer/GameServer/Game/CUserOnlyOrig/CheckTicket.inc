bool CUser::CheckTicket(int ticketIdx)
{
    if (m_gmUserFlag != 0)
        return true;
    if (G_GameWorld()->IsPVPChannel())
        return true;
    // ORIG 0x086785f4：begin sret=-0x14 直构、end sret=-0x10、++it 走 _List_iterator
    // 前缀（无返回槽；const_iterator 转换会多 -0x18 拷贝 ctor@0x384e，P96 保持
    // 非 const iterator；postfix ppEi 另占 -0xc 槽，ORIG 该槽是 ++ 的 sret@0x8391bd2?
    // 实为 `it++` 返 still 见 -0xc：postfix 形）。
    for (std::list<int>::iterator it = m_list8cf80.begin();
         it != m_list8cf80.end(); it++)
    {
        if (*it == ticketIdx)
            return true;
    }
    return false;
