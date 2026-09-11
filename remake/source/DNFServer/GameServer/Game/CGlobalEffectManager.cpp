// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CGlobalEffectManager/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// df_game_r 还原 —— CGlobalEffectManager / CGlobalEffect / TimerGlobalEffectItem*。
// 逐函数依据 docs/class_func_reports/（CGlobalEffectManager/*.md、CGlobalEffect/makeNotifyInfo.md、
// TimerGlobalEffectItemBegin|Timeout/registTimer.md）与 ORIG objdump 逐指令转写。
// 依赖符号（G_GameWorld/PacketGuard/CSystemTime/G_TimerQueue 等）均已由
// GameWorld.cpp / PacketGuard.cpp / CSystemTime.cpp / TimerQueue.cpp 提供。
#include "CGlobalEffectManager.h"

#include <math.h>
#include <string.h>

#include "GameWorld.h"
#include "InterfacePacketBuf.h"
#include "PacketGuard.h"
#include "TimerQueue.h"
#include "CSystemTime.h"
#include "GlobalData.h"

// 值编码常数：ORIG makeNotifyInfo 用 10000.0f 缩放（.rodata 0x8c854b0 = 0x461c4000 = 10000.0f）。
// [W16ASymNameSeg2 2026-09-11 符号名轴] ORIG 无 `_ZL11kValueScale` 实体（全 ELF grep 0 命中）⇒
// 该常量为 .rodata 字面量，内联以消 _ZL11kValueScale（OUR-only 1→0，四数不变 28/67/2/1/0）。

// ============================================================================
// CGlobalEffect（0x2c）
// ============================================================================

#include "CGlobalEffectManager/makeNotifyInfo.inc"


#include "CGlobalEffectManager/CGlobalEffect.inc"


#include "CGlobalEffectManager/dtor_CGlobalEffect.inc"


#include "CGlobalEffectManager/getType.inc"


#include "CGlobalEffectManager/getValue.inc"


#include "CGlobalEffectManager/isTimeout.inc"


// ============================================================================
// CGlobalEffectManager（0x18）
// ============================================================================

#include "CGlobalEffectManager/CGlobalEffectManager.inc"


#include "CGlobalEffectManager/getEffectNum.inc"


#include "CGlobalEffectManager/isAffectedEffect.inc"


#include "CGlobalEffectManager/getEffectValue.inc"


#include "CGlobalEffectManager/makeGlobalEffectInfo.inc"


#include "CGlobalEffectManager/notifyGlobalEffectInfo.inc"


#include "CGlobalEffectManager/registNewEffect.inc"


#include "CGlobalEffectManager/beginReservedEffect.inc"


#include "CGlobalEffectManager/checkTimeout.inc"


// ============================================================================
// TimerGlobalEffectItem（ORIG 静态 helper，登记全局定时器）
// ============================================================================

#include "CGlobalEffectManager/registTimer.inc"

