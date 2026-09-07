// HdrCreate3: ORIG exported symbols, first .h declaration. Evidence: nm ORIG + nm-only
// LinkFinal2: ctor inline 化 —— ORIG _ZN20StatisticsGmCmdPacketC1Ev W @0x0822c4a2 objdump
// 逐指令转写：PacketHeader(0x2722, 0xe)；+0x0a 4B 清零。
// 【推断】是否继承 PacketHeader 以 ORIG PacketHeaderC1Ett 调用为准（nm 仅 ctor 一个符号）。
#ifndef DNF_GAME_STATISTICSGMCMDPACKET_H_
#define DNF_GAME_STATISTICSGMCMDPACKET_H_

#include "PacketHeader.h"

#pragma pack(push, 1)

class StatisticsGmCmdPacket : public PacketHeader
{
public:
    StatisticsGmCmdPacket()   // ORIG W @0x0822c4a2
        : PacketHeader(0x2722, 0xe)   // packetId=0x2722、size=0xe（ORIG C1 0x4(%esp)=0x2722 实证）
    {
        m_characNo = 0;   // +0x0a
    }

    // ORIG oldGmRoutine statisticlog 0x08211f1f 域字段写（SendPacket 0xe 字节）：
    int m_characNo;                // +0x0a（getCurCharacNo）
};

#pragma pack(pop)

static_assert(sizeof(StatisticsGmCmdPacket) == 0xe, "StatisticsGmCmdPacket layout");

#endif  // DNF_GAME_STATISTICSGMCMDPACKET_H_