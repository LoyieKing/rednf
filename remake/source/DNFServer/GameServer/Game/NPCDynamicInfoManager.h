#ifndef GAME_NPCDYNAMICINFOMANAGER_H
#define GAME_NPCDYNAMICINFOMANAGER_H

// ============================================================================
// CNPCDynamicInfoManager —— NPC 动态信息管理器（权威头）。
//
// 布局（ORIG ctor 0x083760f2W 实证）：sizeof 0x18，仅 +0x00 std::map。
// 方法归属：
//   ctor 0x083760f2W / dtor 0x08376106W / getNPCInfo 0x08581948W / add 0x08580668T
//     → CtorProducers_1.cpp（本头仅声明）
//   makeNotiPacketNPCMood 0x085808a6T / onTimer 0x08580722T → NPCDynamicInfoManager.cpp
//
// ORIG 符号（nm -C df_game_r）：
//   _ZNK22CNPCDynamicInfoManager21makeNotiPacketNPCMoodER11PacketGuard T 0x085808a6
//   _ZN22CNPCDynamicInfoManager7onTimerEv                          T 0x08580722
// ============================================================================

#include <map>

#include "NPCDynamicInfo.h"
#include "PacketGuard.h"

class CNPCScript;

class CNPCDynamicInfoManager
{
public:
    CNPCDynamicInfoManager();                       // ORIG 0x083760f2W
    ~CNPCDynamicInfoManager();                      // ORIG 0x08376106W
    CNPCDynamicInfo* getNPCInfo(unsigned int idx);  // ORIG W 0x08581948
    bool add(const CNPCScript* script);             // ORIG T 0x08580668

    // ORIG 0x085808a6 T：put_header(0,0xc3) + 逐 NPC put_short(index)+put_byte(mood)。
    void makeNotiPacketNPCMood(PacketGuard& guard) const;

    // ORIG 0x08580722 T：组包同 makeNotiPacketNPCMood，但循环内 changeMood(false)，
    // 仅 mood 变化时 makeNotiPacketCurMood 并计数；count!=0 才 G_GameWorld()->send_all。
    void onTimer();

    std::map<unsigned int, CNPCDynamicInfo*> m_map;   // +0x00
};

#endif  // GAME_NPCDYNAMICINFOMANAGER_H
