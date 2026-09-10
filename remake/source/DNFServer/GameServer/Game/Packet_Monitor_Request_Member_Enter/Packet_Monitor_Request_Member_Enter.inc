Packet_Monitor_Request_Member_Enter::Packet_Monitor_Request_Member_Enter()
    // [b28c-34] 真语义 bug 修复：ORIG 0x0822b9ce 段 `movl $0x30,0x8(%esp);
    // movl $0x4b7,0x4(%esp)` ⇒ PacketHeader(packetId=0x4b7, packetSize=0x30)；
    // 原写 (0x30,0x4b7) 使包 ID/长度互换。
    : PacketHeader(0x4b7, 0x30)   // ORIG _ZN12PacketHeaderC1Ett
{
    memset(m_data + 0x8, 0, 0x1e);   // ORIG memset(this+0x12, 0, 0x1e)
