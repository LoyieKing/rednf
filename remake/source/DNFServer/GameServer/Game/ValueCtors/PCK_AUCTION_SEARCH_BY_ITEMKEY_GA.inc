PCK_AUCTION_SEARCH_BY_ITEMKEY_GA::PCK_AUCTION_SEARCH_BY_ITEMKEY_GA()
{
    memset(this, 0, 0x81);
    m_header.setCategory(0);
    m_header.setPacketID(6);
    m_header.setSize(0x81);
