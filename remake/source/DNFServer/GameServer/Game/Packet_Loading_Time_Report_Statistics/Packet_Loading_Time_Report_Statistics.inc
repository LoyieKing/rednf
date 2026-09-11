Packet_Loading_Time_Report_Statistics::Packet_Loading_Time_Report_Statistics()
    // [b28c-34] 真语义 bug 修复：ORIG 0x0822c040 段 `movl $0x806,0x8(%esp);
    // movl $0xfb0,0x4(%esp)` ⇒ PacketHeader(packetId=0xfb0, packetSize=0x806)。
    : PacketHeader(0xfb0, 0x806)   // ORIG _ZN12PacketHeaderC1Ett
{
}