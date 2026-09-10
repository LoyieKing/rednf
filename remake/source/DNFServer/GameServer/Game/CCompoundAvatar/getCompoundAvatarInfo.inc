tagCompoundAvatarTable::getCompoundAvatarInfo(int idx) const
{
    // b28d-45 P0/N96 清绑：`register bool invalid asm("al")` 删除，裸短路形
    // （ORIG (idx<1)||(size()<=idx) bool 物化岛 → test/je；body 内 at() 落槽
    // -0xc，*entry==idx je 正跳 return-块）。
    if ((idx < 1) || (static_cast<int>(m_avatars.size()) <= idx))
        return 0;
    const tagCompoundAvatar* entry =
        &m_avatars.at(static_cast<unsigned int>(idx));
    if (entry->m_idx != idx)
        return 0;
    return const_cast<tagCompoundAvatar*>(entry);
