PCK_AUCTION_ASK_REGISTED_ITEM_NUM_GP::PCK_AUCTION_ASK_REGISTED_ITEM_NUM_GP()
{
    memset(this, 0, 0x2d);
    m_header.setCategory(0x12);
    m_header.setPacketID(2);
    m_header.setSize(0x2d);
