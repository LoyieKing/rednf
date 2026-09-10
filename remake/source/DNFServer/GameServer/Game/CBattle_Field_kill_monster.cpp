// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 CBattle_Field_kill_monster/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// CBattle_Field_kill_monster.cpp —— CBattle_Field::kill_monster 完整转写。
//
// ORIG T 0x0830bc78（0x1913 字节）：
//   _ZN13CBattle_Field12kill_monsterER11PacketGuardR11map_monsterRjRK17STKillMonsterData
//   CBattle_Field::kill_monster(PacketGuard&, map_monster&, unsigned int&,
//                               STKillMonsterData const&)
// 依据 docs/class_func_reports/CBattle_Field/kill_monster.md（objdump -d 全量
// 汇编 + Ghidra 反编译 C 双重对照，逐段忠实转写；__PRETTY_FUNCTION__ 取自
// .r 区 0x08c23400 字面量）。
//
// 外部符号调用（桥清零，b21 批次；全部真实类型直调权威头）：
//   CKillMonsterInfo::{Check,Register}{BossKill,APCBossKill,NamedKill}
//     —— CKillMonsterInfo.h，定义见 CKillMonsterInfo_chain.cpp
//   CHellPartyItemStatistic::SendHellPartyItemStatistic —— CHellPartyItemStatistic.h
//   GetInstanceHellPartyItemStatistic                    0x0860fcb6（CHellPartyItemStatistic.h）
//   CDataManager::getDropRate                            0x0830e6d2（CDataManager.cpp）
//   CUser::getAddIndependentDropRateFromPremium          0x0830edaa（CUser.cpp）
//
// 同 TU 依赖的 CBattle_Field 成员（权威头已有纯声明，定义在其它 TU）：
//   KillHellPartyGroupMonsterCnt / checkKilledSpawnBloodMonsters /
//   send_elevator_time_packet / _MakeDropItems / _MakeNotiPacketDieMonster /
//   convertAPCRoleType2MonsterRoleType（定义见 CPartyLink2.cpp）
// ============================================================================

#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <list>
#include <map>
#include <utility>
#include <vector>

#include "CBattle_Field.h"
#include "CParty.h"          // batch-15 dep-downgrade: was via CBattle_Field.h
#include "CGlobalEffectManager.h"   // isAffectedEffect / getEffectValue
#include "CSystemTime.h"            // CSystemTime::getCurTickCount
#include "GlobalData.h"             // s_globalEffectManager_ / s_systemTime_
#include "LogManager.h"             // LogManager::logFormat
#include "CDataManager.h"           // getDropRate / find_monster（桥清零）
#include "CKillMonsterInfo.h"       // 击杀/奖励链六谓词（桥清零）
#include "CHellPartyItemStatistic.h" // 地狱派对统计单例/发送（桥清零）
// ORIG __PRETTY_FUNCTION__（.r 区字面量；重建签名 unsigned int& 与 uint& 文本差异，
// 故按 ORIG 字符串硬编码，供 LogManager/cMyTrace 使用）。
#define BFKM_ROUND(v) ((int)((v) + 0.5f))


namespace
{

// 故按 ORIG 字符串硬编码，供 LogManager/cMyTrace 使用）。
char const s_bfkm_pretty[] =
    "bool CBattle_Field::kill_monster(PacketGuard&, map_monster&, uint&, "
    "const STKillMonsterData&)";
}  // namespace

// ================= kill_monster 主函数 =================

// [b28d-D33 N82/N89 封顶] ORIG 0x0830bc78：1587 vs 1579 条（超大函数 >500，
// §6.2 三件套口径：ndiff 失真，语义以调用点/逐块为准）。语义等价。
#include "CBattle_Field_kill_monster/kill_monster.inc"
}
