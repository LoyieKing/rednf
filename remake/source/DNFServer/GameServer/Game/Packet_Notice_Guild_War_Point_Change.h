// HdrCreate3: ORIG exported symbols, first .h declaration. Evidence: nm ORIG + 源码定义迁移
// 源出处：CUser.cpp:6678 局部最小镜像（ORIG ctor 0x086944e0 W）。
#ifndef DNF_GAME_PACKET_NOTICE_GUILD_WAR_POINT_CHANGE_H_
#define DNF_GAME_PACKET_NOTICE_GUILD_WAR_POINT_CHANGE_H_

#include "PacketHeader.h"

// ORIG W 0x086944e0：PacketHeader(0x417, 0xf) 基类构造（继承自 PacketHeader，+0 即头）。
class Packet_Notice_Guild_War_Point_Change : public PacketHeader
{
public:
    Packet_Notice_Guild_War_Point_Change() : PacketHeader(0x417, 0xf) {}
};

#endif  // DNF_GAME_PACKET_NOTICE_GUILD_WAR_POINT_CHANGE_H_
