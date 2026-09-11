// ORIG 0x822ea84（W 同址 C1/C2）
PCK_AUCTION_SEARCH_BY_ITEMKEY_GP::PCK_AUCTION_SEARCH_BY_ITEMKEY_GP()
{
    memset(this, 0, 0x81);
    m_header.setCategory(0x12);
    m_header.setPacketID(6);
    m_header.setSize(0x81);
}