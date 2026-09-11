Inven_Item_Expand_RedeemInfo CRedeemItem::RedeemItem(int idx)
{
    // [b28d-solo-UserMiscLink] AE：distance 复用 begin 结果（ORIG 0x085f72d8 只调一次
    // begin，旧码二次 m_redeemList.begin() 多 1 槽/4 条）+ memcpy→memmove 对齐 ORIG 符号。
    Inven_Item_Expand_RedeemInfo out;   // ORIG ctor 清零副本
    alter();                            // charac_expand::CData::alter 置脏

    // ORIG 以 const_iterator 定位 idx 后 advance 到非常量迭代器再 erase。
    std::list<Inven_Item_Expand_RedeemInfo>::const_iterator cit =
        _CheckVaildIndex_N_GetIter(idx);
    std::list<Inven_Item_Expand_RedeemInfo>::iterator it = m_redeemList.begin();
    std::advance(it, std::distance<std::list<Inven_Item_Expand_RedeemInfo>::const_iterator>(
                         it, cit));
    if (it != m_redeemList.end())
    {
        std::memmove(&out, &*it, 0x44);
        m_redeemList.erase(it);
    }
    return out;
}