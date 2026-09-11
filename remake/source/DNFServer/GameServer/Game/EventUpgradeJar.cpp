// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 EventUpgradeJar/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================

#include "EventUpgradeJarInfo.h"

#include <algorithm>

#include "DNFLexWrapper.h"   // loadRDARScriptFile/ScanType/ScanInt/ScanFloat（真实符号）

// ---- ST_Event_Upgrade_Jar_EventNPCs_Script 已内联 ctor（头文件）。----

// ============================================================================
// EventUpgradeJarInfo 生命周期
// ============================================================================

#include "EventUpgradeJar/Event_Upgrade_Jar.inc"


#include "EventUpgradeJar/EventUpgradeJarInfo.inc"


#include "EventUpgradeJar/dtor_EventUpgradeJarInfo.inc"


// ============================================================================
// Event_Upgrade_Jar 生命周期
// ============================================================================

#include "EventUpgradeJar/dtor_Event_Upgrade_Jar.inc"


// ============================================================================
// Event_Upgrade_Jar 方法
// ============================================================================

#include "EventUpgradeJar/LoadScript.inc"


#include "EventUpgradeJar/check_requiredRecipe.inc"


#include "EventUpgradeJar/_getFailProb_Fortune.inc"


#include "EventUpgradeJar/_getFailProb_NewNPC.inc"


#include "EventUpgradeJar/_getMeterialCount_NewNPC.inc"


#include "EventUpgradeJar/_getDiscountRate_NewNPC.inc"


#include "EventUpgradeJar/CheckMaterialIndex.inc"


#include "EventUpgradeJar/SettingRecipeNUpConunt.inc"


#include "EventUpgradeJar/GetProbability.inc"


#include "EventUpgradeJar/CheckMaxLuckyLevel.inc"


#include "EventUpgradeJar/GetIndexLuckyStrengtheningDeed.inc"


#include "EventUpgradeJar/getBoosterIndex_byUpgradeCardIndex.inc"


#include "EventUpgradeJar/isFortune.inc"


#include "EventUpgradeJar/isNewNPC.inc"


// ============================================================================
// 自由函数
// ============================================================================

#include "EventUpgradeJar/importEventUpgradeJarScript.inc"


#include "EventUpgradeJar/updateUpgradeTableFromEventUpgradeJarScript.inc"

