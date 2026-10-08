// HdrCreate3: ORIG exported symbols, first .h declaration. Evidence: nm ORIG + 源码定义迁移
// 源出处：CUser.cpp:6678 局部最小镜像（ORIG ctor 0x086944e0 W）。
#ifndef DNF_GAME_PACKET_NOTICE_GUILD_WAR_POINT_CHANGE_H_
#define DNF_GAME_PACKET_NOTICE_GUILD_WAR_POINT_CHANGE_H_

#include "PacketHeader.h"

// ORIG W 0x086944e0：PacketHeader(0x417, 0xf) 基类构造（继承自 PacketHeader，+0 即头）。
// [DNF-FIX 2026-10-08 c32] ORIG CUser::add_guild_pvp_result 0x0865c9e7 写 +0xa（guildkey）、0x0865c9ed 写
//   +0xe（point），SendPacket(...,0xf) ⇒ 字段化（与 shared/packet/include 权威头一致）。旧无字段
//   镜像 sizeof 0xa，调用点经视图 cast 写到对象之外（依赖栈布局的越界写）。
#pragma pack(push, 1)
class Packet_Notice_Guild_War_Point_Change : public PacketHeader
{
public:
    Packet_Notice_Guild_War_Point_Change() : PacketHeader(0x417, 0xf) {}
    unsigned int m_guildKey;   // +0x0a
    char m_point;              // +0x0e
};
#pragma pack(pop)
static_assert(sizeof(Packet_Notice_Guild_War_Point_Change) == 0xf, "Packet_Notice_Guild_War_Point_Change ORIG size 0xf");

#endif  // DNF_GAME_PACKET_NOTICE_GUILD_WAR_POINT_CHANGE_H_
