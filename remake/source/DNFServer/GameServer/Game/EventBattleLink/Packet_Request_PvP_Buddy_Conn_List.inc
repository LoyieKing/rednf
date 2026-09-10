Packet_Request_PvP_Buddy_Conn_List::Packet_Request_PvP_Buddy_Conn_List()
    : PacketHeader(0x1b5a, 0x12)   // ORIG _ZN12PacketHeaderC1Ett(0x1b5a, 0x12)
{
    m_accId = 0;      // ORIG movl $0x0,0xa(%eax)
    m_characNo = 0;   // ORIG movl $0x0,0xe(%eax)
