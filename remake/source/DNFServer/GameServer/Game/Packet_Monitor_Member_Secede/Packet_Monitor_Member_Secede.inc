Packet_Monitor_Member_Secede::Packet_Monitor_Member_Secede()
    // b28c-33 真语义 bug 修复：ORIG 0x0822ba34 为 `movl $0x31,0x8(%esp);
    // movl $0x4bb,0x4(%esp)` ⇒ 第 1 实参=packetId=0x4bb（dispatcher 值域
    // 0x3e8..0x27ff）、第 2 实参=packetSize=0x31；原写法两参颠倒。
    : PacketHeader(0x4bb, 0x31)
{
    memset(m_data + 0x9, 0, 0x1e);   // ORIG memset(this+0x13, 0, 0x1e)
}