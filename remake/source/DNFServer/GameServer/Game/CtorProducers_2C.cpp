// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 CtorProducers_2C/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// CtorProducers_2C.cpp — Ctor_Group2 续批（ContC）：升级/强化/掉落 producer 真实实现
//
// 本 TU 提供以下 ORIG 符号（全部经 docs/class_func_reports 报告 + objdump 转写，
// nm -C --defined-only /home/wangyilei/下载/df_game_r 核实真实存在）：
//   WongWork::CItemUpgrade        LoadUpgradeScript T 0x0854919a /
//                                 Load3rdChronicleUpgradeScript T 0x085491b8
//                                 （C1/C2 已迁回 CItemUpgrade.cpp：P327 GI 键名
//                                  `_GLOBAL__I__ZN8WongWork12CItemUpgradeC2Ev`
//                                  @0x0854b178 + ORIG 地址序 ctor 0x085461c8 <
//                                  dtor 0x085462c6 同属该 TU 段）
//   CItemAmplifier::loadScript    W 0x08376060（_ZN14CItemAmplifier10loadScriptEPKcS1_；
//                                 判据㉓：ORIG nm 实测 W（弱绑定）⇒ 定义已移入
//                                 CItemAmplifier.h 类内，本 TU 不再发射该符号）
//   WongWork::CMCAPManager::init  T 0x080f8aba +
//                                 静态 int pointPerHackType_[3900] B 0x093fad20
//                                 （nm 尺寸 15600=3900×4；double-memset 忠实转写）
//   WongWork::CItemGeneratorMgr   C1/C2 W 0x08374ef4（判据㉓：ORIG nm 实测 W ⇒
//                                 定义已移入 CItemGeneratorMgr.h 类内）/
//                                 initialize T 0x08533bc8
//
// 声明先行（定义待对应域批次，见交付报告 /tmp/df_batches/Ctor2_ContC.md）：
//   WongWork::CItemUpgrade::ProcUpgrade（vtable 槽0 已对齐，报告 1957 行待升级域批次）
//   CMonsterDrop/CObjectDrop/CClearReward/CWorldDrop/CDungeonDrop 各 ctor 与
//   非虚 loadScript 重载（声明见 CItemGenerator.h，定义待掉落域批次）
//   importAmplifyItem / importUpgradeTable（脚本导入域）
// ============================================================================

#include "CSystemTime.h"          // CSystemTime::getCurSec（0x080cbc9e）
#include "GlobalData.h"           // GlobalData::s_systemTime_
#include "TimerDispatcher.h"      // TimerUpdateItemGenerateStatistics::registNextTimer

#include "PvPTypes.h"             // CMTRand（0x9c8 MT19937）
#include "CItemAmplifier.h"       // upgrade_table_t / stAmplifyItemScript_t /
                                  // CItemAmplifier / import{AmplifyItem,UpgradeTable}
#include "CItemGeneratorMgr.h"    // WongWork::CItemGeneratorMgr（含 CItemGenerator.h 全链）
#include "CMCAPManager.h"         // WongWork::CMCAPManager
#include "CDataManager.h"         // WongWork::CItemUpgrade 唯一声明点

#include <cstring>
#include <ctime>

// ============================================================================
// WongWork::CItemUpgrade —— 构造与双装载入口
// ============================================================================

namespace WongWork
{

#include "CtorProducers_2C/LoadUpgradeScript.inc"
}

#include "CtorProducers_2C/Load3rdChronicleUpgradeScript.inc"
}

}  // namespace WongWork

// ============================================================================
// CItemAmplifier::loadScript —— 已移入 CItemAmplifier.h 类内定义
// （判据㉓：ORIG nm `_ZN14CItemAmplifier10loadScriptEPKcS1_` **W** 0x08376060
//  size 0x7e ⇒ 源码为类内定义；函数体逐字不变，仅符号绑定 T→W）
// ============================================================================

// ============================================================================
// WongWork::CMCAPManager —— init 与静态掉落点数表
// ============================================================================

namespace WongWork
{

// ORIG _ZN8WongWork12CMCAPManager17pointPerHackType_E @B 0x093fad20，
// nm st_size 15600 == 3900×sizeof(int)（setPointPerHackType 拷贝循环上界
// 0xf3c==3900 同证）。注：任务书曾写 [3904]，以 ORIG nm 实测尺寸为准取 [3900]。
int CMCAPManager::pointPerHackType_[3900];

#include "CtorProducers_2C/init.inc"
}

}  // namespace WongWork

// ============================================================================
// WongWork::CItemGeneratorMgr —— 构造与 initialize（掉落生成器装配）
// ============================================================================

namespace WongWork
{

// CItemGeneratorMgr::CItemGeneratorMgr 已移入 CItemGeneratorMgr.h 类内定义
// （判据㉓：ORIG nm C1/C2 **W** 0x08374ef4 size 0x18 ⇒ 源码为类内定义；
//  函数体逐字不变，仅符号绑定 T→W）。

#include "CtorProducers_2C/initialize.inc"
}

}  // namespace WongWork
