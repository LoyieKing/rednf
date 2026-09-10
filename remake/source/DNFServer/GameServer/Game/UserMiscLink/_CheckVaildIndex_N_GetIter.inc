CRedeemItem::_CheckVaildIndex_N_GetIter(int idx) const
{
    // 越界（idx<0 / 空 / idx+1>size）→ end()；否则 begin()+idx。
    // ORIG 0x085f7130：`call size` **先**、`mov idx,%edx; add $1,%edx` 后，
    // `cmp %edx,%eax; jge`（有符号，eax=size）⇒ 条件写成 `(int)size < idx+1`
    // 才能让 4.4 右操作数后置并出 jge（写 (size_t)(idx+1) > size 出无符号 jbe
    // + 提前 esi 缓存，帧 0x20 vs ORIG 0x24）。
    if (idx < 0 || m_redeemList.empty() ||
        (int)m_redeemList.size() < idx + 1)
    {
        return m_redeemList.end();
    }
    std::list<Inven_Item_Expand_RedeemInfo>::const_iterator it = m_redeemList.begin();
    std::advance(it, idx);
    return it;
