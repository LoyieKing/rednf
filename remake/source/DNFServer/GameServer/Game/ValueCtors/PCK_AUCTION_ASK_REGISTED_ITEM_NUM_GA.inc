// ORIG 0x822e54e（W 同址 C1/C2）
PCK_AUCTION_ASK_REGISTED_ITEM_NUM_GA::PCK_AUCTION_ASK_REGISTED_ITEM_NUM_GA()
{
    memset(this, 0, 0x3d);
    m_header.setCategory(0);
    m_header.setPacketID(2);
    m_header.setSize(0x3d);
}