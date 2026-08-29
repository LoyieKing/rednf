// HdrCreate3: ORIG exported symbols, first .h declaration. Evidence: nm ORIG + PacketCtor_Classes.cpp
// Packet_DoubleCheck_UDP_Logout：ORIG 弱内联 ctor W @0x0846d5c6。
// has_def=1，两处 cpp 局部定义冲突：
//   - CDoubleConnCheckServerProxy.cpp:42（无基类裸布局）
//   - PacketCtor_Classes.cpp:109（: public PacketHeader，ctor 初始化 PacketHeader(0xbb9,0x10)）
// 以 PacketCtor_Classes.cpp 版为基准（字段偏移一致，+0x0a 起 m_accId）。
#ifndef DNF_GAME_PACKET_DOUBLECHECK_UDP_LOGOUT_H_
#define DNF_GAME_PACKET_DOUBLECHECK_UDP_LOGOUT_H_

#include "PacketHeader.h"

class Packet_DoubleCheck_UDP_Logout : public PacketHeader
{
public:
    Packet_DoubleCheck_UDP_Logout();   // ORIG W @0x0846d5c6

    int m_accId;                 // +0x0a
    unsigned char m_channel;     // +0x0e
    unsigned char m_serverGroup; // +0x0f
} __attribute__((packed));

#endif  // DNF_GAME_PACKET_DOUBLECHECK_UDP_LOGOUT_H_
