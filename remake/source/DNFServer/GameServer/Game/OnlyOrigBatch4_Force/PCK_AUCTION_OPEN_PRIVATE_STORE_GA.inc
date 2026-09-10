PCK_AUCTION_OPEN_PRIVATE_STORE_GA::PCK_AUCTION_OPEN_PRIVATE_STORE_GA()
{
    memset(this, 0, 0xaa);
    setCategory(0);     // 继承 nsl::PACKET_HEADER（m_header 成员已归一为基类）
    setPacketID(0xb);
    setSize(0xaa);
