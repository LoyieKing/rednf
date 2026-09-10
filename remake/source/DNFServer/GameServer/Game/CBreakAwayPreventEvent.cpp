// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CBreakAwayPreventEvent/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// df_game_r 防脱离事件 CBreakAwayPreventEvent 实现。
// 报告：docs/class_func_reports/CBreakAwayPreventEvent/ 全部 6 个方法已读。
//   C1 0x08109dbc / D1 0x08109dd8 / D0 0x08109e08 / StartEvent 0x08109e26 /
//   StartEvent(Word_Param) 0x08109e78（完整实现）/ EndEvent 0x08109ed6。
// 依赖：DB_LoadBreakAwayPreventData::makeRequest(bool)（ORIG T 0x08438920），
//   定义缺失 → 上报 Main 分派实现批次（声明在本 TU，U 引用合规）。
// [GI-ioinit L21 · P338/P327] 补 <iostream> 恢复 __ioinit ⇒ 发射
// `_GLOBAL__I__ZN22CBreakAwayPreventEventC2Ev`（ORIG 0x8109f73）；ctor 已是首强符号。
#include <iostream>

#include "CBreakAwayPreventEvent.h"

#include "GameTypes.h"   // cMyTrace

class DB_LoadBreakAwayPreventData
{
public:
    static void makeRequest(bool flag);   // ORIG T 0x08438920（定义属 DB 任务批次）
};

// ORIG 0x08109dbc：仅基类构造 + vptr 置位。
#include "CBreakAwayPreventEvent/CBreakAwayPreventEvent.inc"
{
}

// ORIG 0x08109dd8 / 0x08109e08：空体 + 虚析构由编译器生成同形。
#include "CBreakAwayPreventEvent/dtor_CBreakAwayPreventEvent.inc"
{
}

// ORIG 0x08109e26：SetEventFlag(true) + cMyTrace(__FUNCTION__, 0x14, 0)
//   ("CBreakAwayPreventEvent Start!")。__FUNCTION__ 名（0x0b 长度符号）。
#include "CBreakAwayPreventEvent/StartEvent.inc"
{
    SetEventFlag(true);
    cMyTrace trace(__FUNCTION__, 0x14, 0);
    trace("CBreakAwayPreventEvent Start!");
}

// ORIG 0x08109e78：SetEventFlag(true) + makeRequest(true) +
//   cMyTrace(__FUNCTION__, 0x1b, 0)("CBreakAwayPreventEvent Start!")。
#include "CBreakAwayPreventEvent/StartEvent_1.inc"
{
    SetEventFlag(true);
    DB_LoadBreakAwayPreventData::makeRequest(true);
    cMyTrace trace(__FUNCTION__, 0x1b, 0);
    trace("CBreakAwayPreventEvent Start!");
}

// ORIG 0x08109ed6：SetEventFlag(false) + makeRequest(false) +
//   cMyTrace(__FUNCTION__, 0x23, 0)("CBreakAwayPreventEvent End!")。
#include "CBreakAwayPreventEvent/EndEvent.inc"
{
    SetEventFlag(false);
    DB_LoadBreakAwayPreventData::makeRequest(false);
    cMyTrace trace(__FUNCTION__, 0x23, 0);
    trace("CBreakAwayPreventEvent End!");
}
