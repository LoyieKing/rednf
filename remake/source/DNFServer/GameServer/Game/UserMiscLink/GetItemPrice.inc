int CRedeemItem::GetItemPrice(int idx)
{
    std::list<Inven_Item_Expand_RedeemInfo>::const_iterator it =
        _CheckVaildIndex_N_GetIter(idx);
    // ORIG 0x085f73cd 调自由函数 _ZSteqI…RKSt14_List_iteratorIT_
    // RKSt20_List_const_iteratorIS3_（iterator 在前、const_iterator 在后）⇒
    // 源码必须写 `end() == it`；写 `it == end()` 走 const_iterator::eq 并多一次
    // iterator→const_iterator 转换构造（+5 条）。
    if (m_redeemList.end() == it)
    {
        return -1;
    }
    return it->m_count40;
