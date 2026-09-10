// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 CHackLog_OverDamage/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// df_game_r 还原 —— CHackLog_OverDamage（过度伤害 hack 检测限值表）
//
// 报告：docs/class_func_reports/CHackLog_OverDamage/*.md（7 份全文，汇编+C）。
// 静态表数据由 ORIG ELF 实测（objdump -s .rodata 0x08c10c68..0x08c10e64，
// /home/wangyilei/下载/df_game_r），与 ORIG 本地符号 _ZL* 一一对应：
//   _ZL33_S_KILL_MONSTER_OVER_DAMAGE_CONST        7 x uint @0x08c10c68
//   _ZL41_S_KILL_MONSTER_OVER_DAMAGE_DUNGEON_RATIO 5 x double @0x08c10ca0
//   _ZL37_S_KILL_MONSTER_OVER_DAMAGE_HACK_TYPE    7 x int @0x08c10cc8
//   _ZL19LEVEL_SECTION_TABLE                     89 x int @0x08c10d00
//
// GetDungeonLimitDamageAndHackType 的 fp 舍入为 RC=truncate（fldcw 0xc in ah
// 后 fistpl），以 C 风格 (long long)(double) 强转等价复现。
// ============================================================================

#include "CHackLog_OverDamage.h"
#include "CHackAnalyzer.h"   // WongWork::ENUM_HACKTYPE 权威枚举
#include "CUser.h"
#include "CParty.h"     // m_dungeon / m_battleField 视图
#include "CDungeon.h"   // get_min_level/get_index
#include "CBattle_Field.h"   // get_dungeon_diff

const unsigned int _S_KILL_MONSTER_OVER_DAMAGE_CONST[7] = {
    600, 1800, 3600, 25000, 55000, 70064, 712640
};   // {258,708,e10,61a8,d6d8,1170,ae60}

// ORIG _ZL41_S_KILL_MONSTER_OVER_DAMAGE_DUNGEON_RATIO（objdump 实测）：
// {2.0,1.8,1.7,1.6,1.0}——第 3/4 项原抄错为 0.075/0.025，已按 ORIG 修正。
const double _S_KILL_MONSTER_OVER_DAMAGE_DUNGEON_RATIO[5] = {
    2.0, 1.8, 1.7, 1.6, 1.0
};

const unsigned int _S_KILL_MONSTER_OVER_DAMAGE_HACK_TYPE[7] = {
    0x65, 0x66, 0x67, 0x68, 0x69, 0x6a, 0x6b
};

const int LEVEL_SECTION_TABLE[89] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
    4, 4, 4, 4, 4, 4, 4, 4,
    5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
    5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5
};

#include "CHackLog_OverDamage/GetDungeonLimitDamageAndHackType.inc"
}

#include "CHackLog_OverDamage/GetDeathTowerLimitDamage.inc"
}

#include "CHackLog_OverDamage/GetDeathTowerOverDamageHackType.inc"
}

#include "CHackLog_OverDamage/GetDungeonLimitMaxDamage.inc"
}

#include "CHackLog_OverDamage/GetHackTypeOverLimitMaxDamage.inc"
}

#include "CHackLog_OverDamage/GetBossTowerLimitDamage.inc"
}

#include "CHackLog_OverDamage/GetBossTowerOverDamageHackType.inc"
}

