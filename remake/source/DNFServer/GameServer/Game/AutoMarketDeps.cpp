// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 AutoMarketDeps/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// df_game_r 还原 —— LinkClosureC 批次：CAutoMarketConditionsControl 单用户耐久通知。
// ORIG 依据：报告 CAutoMarketConditionsControl/SendNotiSetDurability_082f85ea.md
//   _ZN28CAutoMarketConditionsControl20SendNotiSetDurabilityEP5CUser T 0x082f85ea
#include "CDataManager.h"   // CAutoMarketConditionsControl 权威类（CDataManager.h 内声明）

#include "CUser.h"          // CUser::Send(PacketGuard&)
#include "PacketGuard.h"

// MakeNotiSetDurability —— ORIG T 0x082f84f8（LinkMiscSecuTimerSolo.cpp 定义）。
#include "AutoMarketDeps/MakeNotiSetDurability.inc"
#include "AutoMarketDeps/SendNotiSetDurability.inc"
}
