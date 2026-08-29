// HdrCreate3: ORIG exported symbols, first .h declaration. Evidence: nm ORIG + 源码定义迁移
// 源出处：PacketCtor_Classes.cpp:97 局部定义（选为最完整版本；冲突见下注）。
// 冲突：CDoubleConnCheckServerProxy.cpp:28 另有一份同类局部定义（独立 packed POD，
//       m_packetId/m_packetSize/m_reversed1/m_connNo/m_userID/m_channel/m_group，
//       未继承 PacketHeader）。两处布局视图不一致，未改任何 .cpp，待宿主收敛。
#ifndef DNF_GAME_PACKET_DOUBLECHECK_UDP_LOGIN_H_
#define DNF_GAME_PACKET_DOUBLECHECK_UDP_LOGIN_H_

#include "PacketHeader.h"  // 权威定义见 source/shared/packet/include/PacketHeader.h

class Packet_DoubleCheck_UDP_Login : public PacketHeader
{
public:
    Packet_DoubleCheck_UDP_Login();   // W 0x0846d58a
    unsigned int m_userID;   // +0xa
    unsigned char m_channel; // +0xe
    unsigned char m_group;   // +0xf
} __attribute__((packed));
// ORIG ctor: PacketHeader(0xbb8, 0x10), m_userID(0), m_channel(255), m_group(255)

#endif  // DNF_GAME_PACKET_DOUBLECHECK_UDP_LOGIN_H_
