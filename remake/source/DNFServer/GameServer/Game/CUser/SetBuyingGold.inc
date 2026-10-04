void CUser::SetBuyingGold(bool flag)
{
    // ORIG：bool 转换指针形式（mov this; movzbl flag; mov %dl,0x8ead8 完全一致）。
    m_buyingGold = flag;
}