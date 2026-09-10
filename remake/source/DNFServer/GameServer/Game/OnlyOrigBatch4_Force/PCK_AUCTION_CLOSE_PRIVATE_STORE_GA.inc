PCK_AUCTION_CLOSE_PRIVATE_STORE_GA::PCK_AUCTION_CLOSE_PRIVATE_STORE_GA()
{
    memset(this, 0, 0x16);
    m_header.setCategory(0);
    m_header.setPacketID(0xc);
    m_header.setSize(0x16);
