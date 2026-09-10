Packet_CollectItems::Packet_CollectItems() : PacketHeader(0x27e6, 0x39)
{
    // ORIG 存储顺序 +0x12→+0x16→+0xe→+0x1a（函数体赋值序，非成员声明序）
    m_itemIdx = 0;
    m_characNo = 0;
    m_accId = 0;
    m_nameLen = 0;
    memset(m_name, 0, sizeof(m_name));
