Packet_Overflow_Statistic_Add::Packet_Overflow_Statistic_Add()
    // b28c-33 真语义 bug 修复：ORIG 0x0822bdf0 为 `movl $0xd,0x8(%esp);
    // movl $0xc43,0x4(%esp)` ⇒ 第 1 实参=packetId=0xc43、第 2 实参=packetSize=0xd；
    // 原写法两参颠倒。
    : PacketHeader(0xc43, 0xd)
{
}