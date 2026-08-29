// HdrCreate3: ORIG exported symbols, first .h declaration. Evidence: nm ORIG + nm-only
#ifndef DNF_GAME_ST_GUILD_WAR_RANK_INFO_H_
#define DNF_GAME_ST_GUILD_WAR_RANK_INFO_H_

// ORIG W 0x0822ba9a _ZN21ST_Guild_War_Rank_InfoC1Ev（ctor 唯一导出符号）；
// ctor 语义（objdump 0x0822ba9a）：uint(+0)=0、uint(+4)=0、ushort(+8)=0、
// memset(+0xa,0,0x17) → sizeof 0x21（Packet_Notice_Guild_War_Info 9 元素 stride 实证）。
#include <stddef.h>

#pragma pack(push, 1)
class ST_Guild_War_Rank_Info
{
public:
    ST_Guild_War_Rank_Info();  // ORIG W 0x0822ba9a（定义 DisPatcherCore.cpp）

    unsigned int m_field0;    // +0x00（ctor 写 0）
    unsigned int m_field4;    // +0x04（ctor 写 0）
    unsigned short m_field8;  // +0x08（ctor 写 0）
    char m_pad0a[0x17];       // +0x0a..+0x20（ctor memset 0）
};
#pragma pack(pop)

static_assert(sizeof(ST_Guild_War_Rank_Info) == 0x21,
              "ST_Guild_War_Rank_Info ABI drift (Packet_Notice stride 0x21)");

#endif  // DNF_GAME_ST_GUILD_WAR_RANK_INFO_H_
