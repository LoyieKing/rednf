#define _GLIBCXX_IOSTREAM 1
// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CBattle_Field_missing/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// df_game_r CBattle_Field 缺失成员函数实现（新建 TU，宿主登记 CMake）。
//
// 本 TU 实现 CBattle_Field 的 5 个成员函数，取代 GameStubs.cpp 的 asm 桥接
// （宿主登记新 TU 后应删除 GameStubs.cpp:859-869 的 5 个 extern asm 别名 +
// 5 个空实现，避免重复符号 _ZN13CBattle_Field*）。CParty.cpp:45-52 仅是对这
// 些 mangled 符号的 extern 声明（无定义），无需改动，将本 TU 的真实定义解析。
//
// 依据：
//   - ctor  0x082a44fe `_ZN13CBattle_FieldC1Ev`      （子对象构造序 + 标量初值）
//   - dtor  0x082a499c `_ZN13CBattle_FieldD1Ev`      （子对象析构序 = 构造逆序）
//   - pickup_item          0x0830daee  docs/class_func_reports/CBattle_Field/pickup_item.md
//   - reset_field          0x0830a1f6  docs/class_func_reports/CBattle_Field/reset_field.md
//   - reset_hell_party_value 0x0830a57c docs/class_func_reports/CBattle_Field/reset_hell_party_value.md
//
// 说明：
//   * 构造/析构的成员顺序依赖编译器按成员声明序执行的默认行为；内嵌的
//     CBloodRound / CBloodClearRewardData 在 ORIG 与当前布局下均为平凡析构
//     （Inven_Item 无析构函数），ORIG dtor 不为其发出析构调用，故 dtor 无需显式处理。
//   * reset_hell_party_value 中分母 0x08c23d74 == 10000.0f（ORIG .rodata 实证），
//     脚本值经 G_CDataManager()->get_hellparty_script_values() 取得（float 数组）。

#include "CBattle_Field.h"
#include "CParty.h"          // batch-15 dep-downgrade: was via CBattle_Field.h
#include "CVillageMonsterMgr.h"   // 完整类型（reset_field 调用 OnKillVillageMonster 4 参重载）
#include "GlobalData.h"          // GlobalData::s_villageMonsterMgr（ORIG B 0x0941f77c）
#include "LogManager.h"          // logFormat（血轮日志）
#include "TimerQueue.h"          // G_TimerQueue / InsertTimerInMilisecond
#include "GameRand.h"           // get_rand_int

#include "DataManagerInitDepends.h"   // stHellParty_t（get_hellparty_script_values 返回值）

// [b28d-round5-CBattle_Field_missing] 复现 ORIG 脚本源的「基址 + 常数索引」两步寻址（P30）：
// ORIG 三段源装载为 `add $0x4,%edx; flds 0x8(%eax,%edx,4)` / `add $0x8,%edx; flds 0x8(...)` /
// `add $0xc,%edx; flds 0xc(...)`（基址 +0x8/+0x8/+0xc 的 float 数组视角，索引 i+4/i+8/i+0xc）；
// 具名成员直访问 `m_levelDiffRates[i]` 只能折单 disp（disp 0x18 + 索引 i），指令数不同 ⇒
// 以只读视图表达同一内存的数组视角（sizeof/偏移零影响，TU 局部匿名类型，不发射符号）。
namespace
{
// stHellParty_t 的 +0x08 起 float 数组视角（0x8 + (i+4)*4 == m_levelDiffRates[i]，
// 0x8 + (i+8)*4 == m_difficultyRates[i]）。
}

#include "CBattle_Field_missing/CBattle_Field.inc"


#include "CBattle_Field_missing/dtor_CBattle_Field.inc"


#include "CBattle_Field_missing/GetEnableHellDungeon.inc"


#include "CBattle_Field_missing/pickup_item.inc"


#include "CBattle_Field_missing/reset_field.inc"


#include "CBattle_Field_missing/reset_hell_party_value.inc"

// ============================================================================
// == 血轮（Blood Dungeon）链 —— ORIG 报告逐指令转写（2026-08-25 批次）      ==
// 符号清单：/tmp/df_batches/undef_link_syms_2026-08-25.txt 中 CBattle_Field 域；
// 依赖血轮辅助函数（同报告）一并实现，报告均在 docs/class_func_reports/CBattle_Field/。
// ============================================================================

#include "CBattle_Field_missing/isQuestMaze.inc"


#include "CBattle_Field_missing/get_blood_spawn_cnt_of_phase.inc"


#include "CBattle_Field_missing/get_blood_spawn_time_of_phase.inc"


#include "CBattle_Field_missing/get_blood_spawn_phase_total_cnt_of_small_round.inc"


#include "CBattle_Field_missing/get_blood_spawn_time_of_next_small_round.inc"


#include "CBattle_Field_missing/get_blood_round_of_small_round.inc"


#include "CBattle_Field_missing/get_blood_round_difficylty_of_small_round.inc"


#include "CBattle_Field_missing/get_blood_spawn_small_round_interval.inc"


#include "CBattle_Field_missing/checkBloodSpawnOrder.inc"


#include "CBattle_Field_missing/BloodMonsterSpawnPacket.inc"


#include "CBattle_Field_missing/BloodPhaseSpawn.inc"


#include "CBattle_Field_missing/BloodPhaseSpawnTimerResgister.inc"


#include "CBattle_Field_missing/BloodRoundIntervalPacket.inc"


#include "CBattle_Field_missing/BloodCrazyMonsterSpawnPacket.inc"


#include "CBattle_Field_missing/BloodSpawnTimerManager.inc"


#include "CBattle_Field_missing/getNextRoundDifficulty.inc"


#include "CBattle_Field_missing/forceValuesToChangeRound.inc"


#include "CBattle_Field_missing/sendUltimateRandomDifficulty.inc"


#include "CBattle_Field_missing/sendUltimateRandomDifficulty_1.inc"


#include "CBattle_Field_missing/startUltimateRound.inc"


#include "CBattle_Field_missing/getUltimateRewardItem.inc"


#include "CBattle_Field_missing/setRewarItemCopy.inc"


#include "CBattle_Field_missing/setRewardItemAddInfo.inc"


#include "CBattle_Field_missing/onFinishBloodRound.inc"


#include "CBattle_Field_missing/get_blood_spawn_cnt_of_small_round.inc"


#include "CBattle_Field_missing/getNextSamllRoundOrder.inc"

