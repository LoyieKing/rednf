bool CRedeemItem::loadData(CUser* pUser, char* pData)
{
    (void)pUser;
    _reset();   // ORIG 0x085f7846：虚调 vtable slot7
    const RedeemEntryView* buf =
        reinterpret_cast<const RedeemEntryView*>(pData);   // ORIG 0x085f7848
    for (unsigned int i = 0; i < ((const RedeemListHeadView*)buf)->m_count; ++i)
    {
        if (buf[i].m_head.m_check == 0)   // ORIG 0x085f7867 mov 0xa(%eax,%edx,1)
        {
            alter();   // ORIG 0x085f7875: charac_expand::CData::alter()
        }
        else
        {
            // ORIG 0x085f788c：lea 0x8(%eax),%edx 直传源地址（无临时对象）。
            m_redeemList.push_back(
                *reinterpret_cast<const Inven_Item_Expand_RedeemInfo*>(
                    buf[i].m_body.m_info));
        }
    }
    return true;
