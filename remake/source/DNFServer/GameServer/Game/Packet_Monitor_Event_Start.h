// HdrCreate3: ORIG exported symbols, first .h declaration.
// Evidence: nm ORIG + 复制定义（has_def=1，源 GmCmds_F.cpp:38-45 / LinkMisc4_Data.cpp:75-82，
// 注明其复刻自 Monitor/DNFPacketTranslater.h:439 权威布局）
#ifndef DNF_GAME_PACKET_MONITOR_EVENT_START_H_
#define DNF_GAME_PACKET_MONITOR_EVENT_START_H_

#include "PacketHeader.h"

// ORIG W 0x084b4184 _ZN23Packet_Monitor_Event_StartC1Ev
// 权威布局（Monitor/DNFPacketTranslater.h:439）：packed，sizeof == 0x12。
// 注意：GmCmds_F.cpp:38 与 LinkMisc4_Data.cpp:75 存在本地同构定义（未删除），
// 引入本头的 TU 必须避免再次重复定义 —— 冲突已记录，cpp 侧清理由宿主统一安排。
class Packet_Monitor_Event_Start : public PacketHeader
{
public:
    Packet_Monitor_Event_Start();     // ORIG W 0x084b4184：PacketHeader(0x44f, 0x12)
    unsigned int m_field1;            // +0xa eventType/eventCode
    unsigned short m_field2;          // +0xe param1
    unsigned short m_field3;          // +0x10 param2
} __attribute__((packed));

#endif  // DNF_GAME_PACKET_MONITOR_EVENT_START_H_
