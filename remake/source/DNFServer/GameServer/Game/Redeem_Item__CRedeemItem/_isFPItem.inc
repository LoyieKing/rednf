bool CRedeemItem::_isFPItem(const Inven_Item& item) const
{
    // ORIG：无局部槽，两次完整读 0x2(%eax)，无符号 jbe/ja（P48 双读）
    return item.m_addInfo > 0x1963 && item.m_addInfo <= 0x1b57;
