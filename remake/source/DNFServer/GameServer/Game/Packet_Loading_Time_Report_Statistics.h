#ifndef DNF_GAME_PACKET_Packet_Loading_Time_Report_Statistics_H_
#define DNF_GAME_PACKET_Packet_Loading_Time_Report_Statistics_H_

// LinkHelper 核销批次（/tmp/undef_r21.txt）：ORIG ctor W @0x0822c040
// _ZN37Packet_Loading_Time_Report_StatisticsC1Ev，objdump 逐指令转写。
// 布局：PacketHeader(0xa) + m_data，sizeof=0x806。
#include <string.h>

#include "PacketHeader.h"

#pragma pack(push, 1)

class Packet_Loading_Time_Report_Statistics : public PacketHeader
{
public:
    Packet_Loading_Time_Report_Statistics();   // ORIG W @0x0822c040

    char m_data[0x806 - 0xa];   // +0x0a 起
};

#pragma pack(pop)

static_assert(sizeof(Packet_Loading_Time_Report_Statistics) == 0x806, "Packet_Loading_Time_Report_Statistics layout");

#endif  // DNF_GAME_PACKET_Packet_Loading_Time_Report_Statistics_H_
