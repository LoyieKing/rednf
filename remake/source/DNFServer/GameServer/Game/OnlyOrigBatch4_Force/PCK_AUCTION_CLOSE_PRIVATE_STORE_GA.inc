// ORIG W 0x085cb3dc（C2 同址）：PACKET_HEADER C1 → memset(this,0,0x16) →
// setCategory(0)/setPacketID(0xc)/setSize(0x16)。
PCK_AUCTION_CLOSE_PRIVATE_STORE_GA::PCK_AUCTION_CLOSE_PRIVATE_STORE_GA()
{
    memset(this, 0, 0x16);
    m_header.setCategory(0);
    m_header.setPacketID(0xc);
    m_header.setSize(0x16);
}