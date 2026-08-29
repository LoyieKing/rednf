// LinkHelper 核销批次 —— Packet_Monitor_Request_Charac_Info ctor（ORIG W @0x0822c5d2）。
#include "Packet_Monitor_Request_Charac_Info.h"

Packet_Monitor_Request_Charac_Info::Packet_Monitor_Request_Charac_Info()
    : PacketHeader(0x27de, 0x23)   // ORIG _ZN12PacketHeaderC1Ett
    , m_field1f(0)                 // movl $0x0,0x1f(%eax)
{
    memset(m_data, 0, 0x15);       // ORIG memset(this+0xa, 0, 0x15)
}
