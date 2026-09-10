unsigned char Inven_Item::GetTradeLimitCount() const
{
    return (m_attr & 0xe0) >> 5;
