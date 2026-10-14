// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CHackLog_InvalidAreaMove/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// [GI-ioinit L21 · P338/P327] 补 <iostream> 恢复 __ioinit ⇒ 发射
// `_GLOBAL__I__ZN24CHackLog_InvalidAreaMove11GetHackTypeEv`（ORIG 0x828619a）；
// GetHackType 定义已是本 TU 首强符号。
#include <iostream>

#include "CHackLog_InvalidAreaMove.h"

// 全部方法按 ORIG df_game_r（elf32-i386）逐指令反汇编转写。
// 反汇编命令：objdump -d .origin_1031/build/dnf_data/home/template/neople/game/df_game_r
// 地址与符号见各方法注释。

#include "CHackLog_InvalidAreaMove/checkSeriaRoom.inc"


#include "CHackLog_InvalidAreaMove/GetHackType.inc"


#include "CHackLog_InvalidAreaMove/IsMovingBetweenWestCoastAndGent.inc"


#include "CHackLog_InvalidAreaMove/IsGentEntranceInWestCoast.inc"


#include "CHackLog_InvalidAreaMove/IsWestCostEntranceInGent.inc"


#include "CHackLog_InvalidAreaMove/IsMovingAboutPowerWar.inc"


#include "CHackLog_InvalidAreaMove/IsMovingInGuildAgit.inc"


#include "CHackLog_InvalidAreaMove/IsExceptionMove.inc"


#include "CHackLog_InvalidAreaMove/IsMovingBetweenSeriaRoomAndGuildAgit.inc"



#include "CHackLog_InvalidAreaMove/IsInvalidMovableArea.inc"


#include "CHackLog_InvalidAreaMove/isMovableRoute.inc"


#include "CHackLog_InvalidAreaMove/IsHendonMyre.inc"


#include "CHackLog_InvalidAreaMove/IsWarfare.inc"


#include "CHackLog_InvalidAreaMove/IsGuildAgit.inc"


#include "CHackLog_InvalidAreaMove/IsSeriaRoom.inc"


#include "CHackLog_InvalidAreaMove/Is_restrictive_commercial_transaction_zone.inc"

