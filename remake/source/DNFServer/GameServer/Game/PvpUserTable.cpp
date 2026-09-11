#define _GLIBCXX_IOSTREAM 1
// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 PvpUserTable/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// [G5 2026-09-10 · L12 round3 复核] 2 件全核（p167 + 判据⑪）：
//   · GetTeamVsTeamPoint：判据⑪ = (a) 相对序差（我方 -0x10..-0x4c ↔ ORIG -0xc..-0x48，
//     浅端多 4B/深端多 4B）；P227 全表达式临时链试验（ORIG `fstpl -0x48` 在 ctor 前）
//     实测四数与槽表逐值不变 ⇒ 回滚；残余 N85（fucompp/fnstsw/sahf ↔ 4.4 fucomip/fstp）
//     + N82 槽组（帧 0x70↔0x60），p167 CLEAR 7/7 ⇒ 语义完成。
//   · GetResult：判据⑪ = (b) 整帧平移（槽集合全等）⇒ N126 封顶，p167 仅别名差。

#include <algorithm>
#include <cstring>

#include "PvPTypes.h"  // PvpUserTable 权威完整定义（本 TU 唯一实现归属）

#include "CUserCharacInfo.h"
#include "GameWorld.h"

class CUser;

// cMyTrace 权威声明见 GameTypes.h（经 GameWorld.h 链包含），本地类已删除（§7）。


#include "PvpUserTable/PvpUserTable.inc"


#include "PvpUserTable/dtor_PvpUserTable.inc"


#include "PvpUserTable/Clear.inc"


#include "PvpUserTable/Set.inc"


#include "PvpUserTable/IsValidTeam.inc"


#include "PvpUserTable/IsValidConvertedGrade.inc"


#include "PvpUserTable/GetTeamUserCount.inc"


#include "PvpUserTable/GetExpectVal.inc"


#include "PvpUserTable/Calculate.inc"


#include "PvpUserTable/GetTeamVsTeamPoint.inc"


#include "PvpUserTable/GetGhostVsTeamPoint.inc"


#include "PvpUserTable/GetResult.inc"


#include "PvpUserTable/GetLeavePenalty.inc"


#include "PvpUserTable/GradeFilter.inc"


#include "PvpUserTable/CheckDan.inc"


#include "PvpUserTable/SetWinTeam.inc"


#include "PvpUserTable/GetWinTeam.inc"


#include "PvpUserTable/GetUserVsTeamPoint.inc"

