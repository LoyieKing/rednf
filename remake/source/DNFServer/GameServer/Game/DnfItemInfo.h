#ifndef GAME_DNFITEMINFO_H_
#define GAME_DNFITEMINFO_H_

// 布局依据 ORIG 反汇编（df_game_r W 符号：ctor 0x84e910a / reset 0x84e9094）：
//   pack(1)，sizeof 0x35。+0x13..0x1c 为 10 字节保留区（reset 不触碰，不声明语义）。
//   +0x1d RandomOption（Inven_Item.h 权威，0xe 字节）+0x2b UpgradeSeparateInfo
//   +0x2c ReservedCapacity。字段命名/布局与 Guild/GuildCargo.h 权威版一致。
#include "Inven_Item.h"

#pragma pack(push,1)
class DnfItemInfo
{
public:
    // ORIG 0x84e910a _ZN11DnfItemInfoC1Ev：m_up@+0x2b/m_res@+0x2c 成员 ctor 后调 reset()
    DnfItemInfo();
    // ORIG 0x84e9094 _ZN11DnfItemInfo5resetEv
    void reset();

    unsigned char m_seal;          // +0x00
    unsigned int m_itemId;         // +0x01
    unsigned char m_attr;          // +0x05
    unsigned int m_addInfo;        // +0x06
    unsigned short m_endurance;    // +0x0a
    unsigned int m_extendInfo;     // +0x0c
    unsigned char m_abilityType;   // +0x10
    unsigned short m_abilityValue; // +0x11
    char m_pad[0x1d - 0x13];       // +0x13..0x1c（ORIG reset 不触碰）
    RandomOption m_opt;            // +0x1d（0xe 字节）
    UpgradeSeparateInfo m_up;      // +0x2b
    ReservedCapacity m_res;        // +0x2c
};
#pragma pack(pop)

#endif  // GAME_DNFITEMINFO_H_
