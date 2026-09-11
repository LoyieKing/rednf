// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CExpertJobList/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================

#include "CExpertJobList.h"

#include "DNFLexWrapper.h"   // loadRDARScriptFile / ScanType / ScanInt / ScanStr / ScanFloat
#include "LogManager.h"      // LogManager::logFormat
#include <stdio.h>
#include "NpcScript.h"

// ---- 本地 STScriptFileList（与 CItemShop.cpp / STScriptFileList.cpp 同布局：
//      +0x00 std::map<int,std::string>，大小 0x18）----


// ORIG 全局（B g_expertJobScriptBaseDirectory 0x094f7660[0x100] /
// g_expertJobScriptFileList 0x094f7760 = 0x18 STScriptFileList 对象）。
// 由 initExpertJobScript（InitScriptProducers_5.cpp）填充；init() 遍历使用。
// b31-SweepCExpertJobList [P327]：定义序照 ORIG —— ORIG GI 键名
// _GLOBAL__I_g_expertJobScriptBaseDirectory（0x0896dbb0，size 0x1c）取「本 TU 内
// 第一个强符号·外部链接·定义」实体；我方原序（FileList 在前）产
// _GLOBAL__I_g_expertJobScriptFileList（仅 NEW，ORIG 无此名）。BaseDirectory 是
// POD 数组（零初始化、无动态构造）⇒ 前移不改变本 TU 构造序，其余符号集不变。
char g_expertJobScriptBaseDirectory[0x100];
STScriptFileList g_expertJobScriptFileList;

// [potion info] 段读取并写出的炼金术值表。
// ORIG 为单一全局 g_alchemistInfo（B 0x094f7778，nm 尺寸 8）；[0] 即低半区
// g_alchemistInfo，[1] 即未命名全局 _DAT_094f777c（见 ImportExpertJobScript.md
// [potion info] 反编译）。本 TU 为唯一定义/写点。
int g_alchemistInfo[2];

// ============================================================================
// expert_job::get_disjoint_additional_items（ORIG 0x896d75d）。
// 在 multimap 中按 key 取 equal_range，将每个 mapped stItemSelectRate 压入 out；
// 未找到返回 false，否则返回 true。供 [disjoint result] 校验/填充附加/大奖物品。
// b28d-solo-CExpertJobList 判词（AE）：P222 正向包裹（命中块 fall-through、false 归
// 共享尾 0x0896d829）+ P13 缓存 upper_bound（ORIG 0x0896d7e2 循环初始化区一次求值、
// 条件只比较）；64=64。
// ============================================================================
namespace expert_job
{
#include "CExpertJobList/get_disjoint_additional_items.inc"

}

#include "CExpertJobList/STExpertJobScript.inc"


#include "CExpertJobList/Clear.inc"


#include "CExpertJobList/Clear_1.inc"

#include "CExpertJobList/STExpertJobEtcScript.inc"


#include "CExpertJobList/ImportExpertJobScript.inc"


#include "CExpertJobList/ImportExpertJobEtcScript.inc"


#include "CExpertJobList/CExpertJobList.inc"


// [b28d-Main·检查点21] CExpertJobList/STExpertJobEtcScript 的 dtor 已删（ORIG 无符号）。

#include "CExpertJobList/init.inc"


#include "CExpertJobList/destroy.inc"


#include "CExpertJobList/get.inc"


#include "CExpertJobList/isExpertJobLimitLevel.inc"

