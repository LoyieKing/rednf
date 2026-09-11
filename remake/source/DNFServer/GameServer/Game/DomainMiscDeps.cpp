#define _GLIBCXX_IOSTREAM 1
// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 DomainMiscDeps/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// df_game_r Game/ DomainMiscDeps 专属 TU。
// 覆盖符号（全部 T/W，地址经 ORIG df_game_r nm/objdump 核实）：
//   IndependentDropParameterScript::{load_independent_drop_list,
//       ImportIndependentDrop, importIndependentDropParameterScript}
//       0x0897c0d8 / 0x0897c398 / 0x0897c1c6
//   PvPSkillTreeParameterScript::{load_pvp_skill_tree_list,
//       importPairPvPSkillPointScript, importPvPSkillTreeParameterScript}
//       0x08a5ce28 / 0x08a5e226 / 0x08a5cf16
//   RegenerationROI::_0_Load_RegenerationRandomOptionItem 0x085f963e
//   TimerNPCMoodChange::registNextTimer(long)            0x086381ec（纯 ret）
//   TimerUpdateItemGenerateStatistics::registNextTimer(long) 0x08637318
//   OS_API::Sleep(int)                                   0x0858c4e0
//   （DNFFLib::Make_Dir 0x08108ad7 归属 ServerCommon/DNFFunctionLib.cpp，不在此重复）
// 逐函数对照 docs/class_func_reports/<类>/<方法>.md 的 ## 汇编 与
// b28d-solo-DomainMiscDeps 判词汇总（8 件，全部已读 ORIG 汇编 + Ghidra C）：
//   1. RegenerationROI::_0_Load_RegenerationRandomOptionItem 0x085f963e
//      → FIXED（P225 while+return 复现 `cmpl;setne;test;al;je` 无槽物化）。
//   2. stMonsterIndependentDrop_t::C1ERKS_ 0x08376bb2 → N90 封顶：
//      ORIG(4.1) 对 int[11] 数组成员隐式拷贝分块 5+4+1+1（组间重载 src/dst
//      基址），4.4 隐式拷贝恒单次连发。实测 6 形（隐式/拆成员/匿名 struct/
//      union 双视图/初始化列表 trivial/显式体内分块）均不能闭合：唯一能产
//      分块的是显式拷贝构造体内分块语句（探针 S6 逐条全等），但 vector 拷贝
//      构造必须在初始化列表 ⇒ 分块必落在 vector 之后，顺序与 ORIG 相反；
//      且 union 成员带用户拷贝构造被 C++ 禁止。拆真实成员会破坏动态下标
//      `m_rate[jobIdx]`（independentItemRateControl 0x08349796 单条 scaled）。
//   3. stMonsterIndependentDrop_t::aSERKS_ 0x083a3942 → N90 封顶（同上）。
//   4. load_pvp_skill_tree_list 0x08a5ce28 → FIXED（P50/P314 非 const
//      iterator 先声明后赋值 + 空名 break 守卫）。
//   5. importIndependentDropParameterScript 0x0897c1c6 → DIFF 残余
//      （p167 CLEAR 20/20）：已修 P222 正向包裹、unsigned lea 加法形、
//      make_pair<unsigned&&> 形；残余 = count 右值引用物化临时 -0x10（4.1 直传
//      count 槽地址）+ rate 槽 -0xc（4.1 -0x40）⇒ N82 槽分配族。
//   6. importPairPvPSkillPointScript 0x08a5e226 → DIFF 残余
//      （p167 CLEAR 33/33）：已修 row 声明序（帧 0xd0 与 ORIG 同）、left 两次
//      size() 内联（P48）、slotsPerJob 非 const、ScanType 失败 break+尾 return；
//      残余 2 条（N82/N93 槽向）。
//   7. ImportIndependentDrop 0x0897c398 → 真 bug 修复 + DIFF 残余
//      （p167 CLEAR 57/57）：旧代码 m_rate 索引整体错一格（rate[4..7] 循环 +
//      rate[8] + rate[9]=0），ORIG 为 rate[4] 单读(0xa5) + 循环 rate[5..8]
//      (0xab) + rate[9] 单读(0xb0) + rate[10]=0 ⇒ 掉落率/数量字段错位；
//      已补缺失的 1 个 ScanInt。残余 = 局部槽基 -0xa0 vs -0x9c（N82）。
//   8. importPvPSkillTreeParameterScript 0x08a5cf16 → DIFF 残余
//      （p167 CLEAR 116/116）：已修 operator!= 正极性（P220/P47）、
//      pair<int,int> 循环内默认构造+成员赋值+push_back(const&)（P135）、
//      去 table 引用缓存复现 3 次 perIt->second 访问、ScanType 失败 break+
//      尾 return；残余 3 条（N104 EH 尾块序 / 槽位）。
// ## 反编译 C 转写；报告缺失处以 objdump 反汇编为准并标注。

#include <algorithm>
#include <dirent.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>
#include <utility>
#include <vector>

#include "CDataManager.h"       // IndependentDrop/PvPSkillTree/RegenerationROI 权威布局
#include "DNFLexWrapper.h"      // loadRDARScriptFile/ScanType/ScanInt/ScanFloat
#include "LogManager.h"
#include "GlobalData.h"         // GlobalData::s_systemTime_
#include "CSystemTime.h"        // CSystemTime::getCurSec
#include "TimerQueue.h"         // G_TimerQueue/TimerEntry::InsertTimer
#include "TimerDispatcher.h"    // TimerNPCMoodChange/TimerUpdateItemGenerateStatistics
#include "NpcScript.h"

// ---- ORIG B 全局脚本文件列表（定义 InitScriptProducers_5.cpp；本 TU 引用）----
// 本地 STScriptFileList 与 STScriptFileList.cpp / InitScriptProducers_5.cpp /
// NpcScript.h 同布局（+0x00 std::map<int,std::string>, sizeof 0x18），方法调用


extern char g_independentDropScriptBaseDirectory[0x100];   // B 0x094f7840
extern STScriptFileList g_independentDropScriptFileList;   // B 0x094f7940
extern char g_pvpSkillTreeScriptBaseDirectory[0x100];      // B 0x09500240
extern STScriptFileList g_pvpSkillTreeScriptFileList;      // B 0x09500340

#include "DomainMiscDeps/importRegenerationRandomOptionScript.inc"
                                         const char* path);

// ===========================================================================
// OS_API::Sleep —— ORIG 0x0858c4e0：usleep(ms*1000)。
// W7_BuildHarmonize：唯一 owner 为 LinkFinalDeps.cpp（同 ORIG 符号），此处
// 原本并发批次撞车重复定义已移除，仅保留注释说明。
// ===========================================================================

// ===========================================================================
// DNFFLib::Make_Dir —— 唯一定义在 ServerCommon/DNFFunctionLib.cpp
// （ORIG 0x08108ad7：opendir 失败时 mkdir(dir,0777)；此处仅声明引用，不重复实现）。
// ===========================================================================
// 定时器注册
// ===========================================================================

#include "DomainMiscDeps/registNextTimer.inc"



#include "DomainMiscDeps/_0_Load_RegenerationRandomOptionItem.inc"


// ===========================================================================
// IndependentDropParameterScript —— 独立掉落域
// ===========================================================================

#include "DomainMiscDeps/importIndependentDropParameterScript.inc"


#include "DomainMiscDeps/load_independent_drop_list.inc"


#include "DomainMiscDeps/ImportIndependentDrop.inc"


// ===========================================================================
// PvPSkillTreeParameterScript —— PvP 技能树域
// ===========================================================================

#include "DomainMiscDeps/importPvPSkillTreeParameterScript.inc"


#include "DomainMiscDeps/importPairPvPSkillPointScript.inc"


#include "DomainMiscDeps/load_pvp_skill_tree_list.inc"

