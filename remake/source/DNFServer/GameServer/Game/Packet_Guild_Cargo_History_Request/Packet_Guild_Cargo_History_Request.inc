Packet_Guild_Cargo_History_Request::Packet_Guild_Cargo_History_Request()
    : PacketHeader(0x70b, 0x12)   // ORIG _ZN12PacketHeaderC1Ett
{
    // ORIG：+0xa/+0xe 两个 dword 清零（2×movl，非 call memset）。
    m_guildKey = 0;
    m_charNo = 0;
