Packet_Monitor_Set_Sub_Guild_Master::Packet_Monitor_Set_Sub_Guild_Master()
    : PacketHeader(0x430, 0x31)   // ORIG _ZN12PacketHeaderC1Ett
{
    // ORIG ctor 逐指令：movl $0,this+0xa；movl $0,this+0xe；
    // memset(this+0x12, 0, 0x1e) —— 覆盖 [this+0xa, this+0x30)，不含 +0x30(m_grade)。
    m_charNo = 0;                       // this+0xa
    m_guildKey = 0;                     // this+0xe
    memset(m_name, 0, 0x1e);            // this+0x12 起 0x1e 字节
}