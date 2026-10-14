#ifndef GAME_CHACKLOG_OVERDAMAGE_H_
#define GAME_CHACKLOG_OVERDAMAGE_H_

// ============================================================================
// df_game_r 还原 —— CHackLog_OverDamage（全静态方法，无数据成员/this 参与）
//
// ORIG 符号（nm -S .origin_1031/build/dnf_data/home/template/neople/game/df_game_r 全 T）：
//   GetDungeonLimitDamageAndHackType(CUser*, unsigned int&,
//                                    WongWork::ENUM_HACKTYPE&)  T 0x08286790
//   GetDeathTowerLimitDamage()      T 0x08286984
//   GetDeathTowerOverDamageHackType() T 0x0828698e
//   GetDungeonLimitMaxDamage()      T 0x08286998
//   GetHackTypeOverLimitMaxDamage() T 0x082869a2
//   GetBossTowerLimitDamage()       T 0x082869ac
//   GetBossTowerOverDamageHackType() T 0x082869b6
//
// 静态表（ORIG .rodata 本地符号 _ZL*，值由 objdump 实测提取）：
//   _ZL33_S_KILL_MONSTER_OVER_DAMAGE_CONST       0x08c10c68 (7 x int)
//   _ZL41_S_KILL_MONSTER_OVER_DAMAGE_DUNGEON_RATIO 0x08c10ca0 (5 x double)
//   _ZL37_S_KILL_MONSTER_OVER_DAMAGE_HACK_TYPE   0x08c10cc8 (7 x int)
//   _ZL19LEVEL_SECTION_TABLE                     0x08c10d00 (89 x int)
//
// 报告依据：docs/class_func_reports/CHackLog_OverDamage/*.md（7 份全文）。
// 定义见 CHackLog_OverDamage.cpp。
// ============================================================================

#include "CHackAnalyzer.h"   // WongWork::ENUM_HACKTYPE 权威枚举

class CUser;

class CHackLog_OverDamage
{
public:
    static void GetDungeonLimitDamageAndHackType(CUser* user,
                                                 unsigned int& limit,
                                                 WongWork::ENUM_HACKTYPE& type);
    static unsigned int GetDeathTowerLimitDamage();
    static WongWork::ENUM_HACKTYPE GetDeathTowerOverDamageHackType();
    static unsigned int GetDungeonLimitMaxDamage();
    static WongWork::ENUM_HACKTYPE GetHackTypeOverLimitMaxDamage();
    static unsigned int GetBossTowerLimitDamage();
    static WongWork::ENUM_HACKTYPE GetBossTowerOverDamageHackType();
};

#endif  // GAME_CHACKLOG_OVERDAMAGE_H_
