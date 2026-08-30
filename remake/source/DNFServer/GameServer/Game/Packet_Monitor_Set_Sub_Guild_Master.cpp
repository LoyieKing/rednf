// ============================================================================
// LinkHelper 核销批次 —— Packet_Monitor_Set_Sub_Guild_Master ctor（ORIG W @0x0822b978 objdump 逐指令转写）。
// ============================================================================
#include "Packet_Monitor_Set_Sub_Guild_Master.h"

Packet_Monitor_Set_Sub_Guild_Master::Packet_Monitor_Set_Sub_Guild_Master()
    : PacketHeader(0x430, 0x31)   // ORIG _ZN12PacketHeaderC1Ett
{
    // ORIG ctor 逐指令：movl $0,this+0xa；movl $0,this+0xe；
    // memset(this+0x12, 0, 0x1e) —— 覆盖 [this+0xa, this+0x30)，不含 +0x30(m_grade)。
    *(unsigned int*)(m_data + 0) = 0;   // 清 this+0xa（m_charNo）
    *(unsigned int*)(m_data + 4) = 0;   // 清 this+0xe（m_guildKey）
    memset(m_data + 8, 0, 0x1e);        // 清 this+0x12 起 0x1e 字节（m_name）
}
