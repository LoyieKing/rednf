Inven_Item_Expand_RedeemInfo CRedeemItem::GetRedeemInfoR(int idx) const
{
    Inven_Item_Expand_RedeemInfo out;   // ORIG ctor 清零副本
    std::list<Inven_Item_Expand_RedeemInfo>::const_iterator it =
        _CheckVaildIndex_N_GetIter(idx);
    if (it != m_redeemList.end())
    {
        std::memmove(&out, &*it, 0x44);
    }
    return out;
