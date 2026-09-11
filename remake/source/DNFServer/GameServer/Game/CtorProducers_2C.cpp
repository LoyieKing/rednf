// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CtorProducers_2C/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
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


#include "CtorProducers_2C/Load3rdChronicleUpgradeScript.inc"


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


}  // namespace WongWork
