// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CGmRegistEvent/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// df_game_r GM 注册事件 CGmRegistEvent 实现。
// 报告：docs/class_func_reports/CGmRegistEvent/ 全部 6 个方法
// （CGmRegistEvent / ~CGmRegistEvent / ~CGmRegistEvent_0811ccc4 /
//  StartEvent / StartEvent_0811cd2c / EndEvent）。
// [GI-ioinit L21 · P338/P327] 补 <iostream> 恢复 __ioinit ⇒ 发射
// `_GLOBAL__I__ZN14CGmRegistEventC2Ev`（ORIG 0x811cdf4）；ctor 已是首强符号。
#include <iostream>

#include "CGmRegistEvent.h"

#include "GameTypes.h"             // cMyTrace
#include "W8LT_DBTask.h"           // DB_UpdateGMData::makeRequest

// ---------------------------------------------------------------------------
// ORIG 0x0811cc78：仅基类构造 + vptr 置位（无自有成员）。
#include "CGmRegistEvent/CGmRegistEvent.inc"
{
}

// ORIG 0x0811cc94 / 0x0811ccc4：D1 重置 vptr 后调 CEventBase::~CEventBase；
// D0 调 D1 后 operator delete。空体 + 虚析构由编译器生成同形。
#include "CGmRegistEvent/dtor_CGmRegistEvent.inc"
{
}

// ---------------------------------------------------------------------------
// ORIG 0x0811cce2：DB_UpdateGMData::makeRequest(true) +
// cMyTrace(__PRETTY_FUNCTION__,0x16,0)
// ("CGmRegistEvent Start!")（无 SetEventFlag）。
#include "CGmRegistEvent/StartEvent.inc"
{
    DB_UpdateGMData::makeRequest(true);
    cMyTrace tr(__PRETTY_FUNCTION__, 0x16, 0);
    tr("CGmRegistEvent Start!");
}

// ---------------------------------------------------------------------------
// ORIG 0x0811cd2c：同 StartEvent()（file 串
// "virtual void CGmRegistEvent::StartEvent(Word_Param)"，行 0x1d）。
#include "CGmRegistEvent/StartEvent_1.inc"
{
    DB_UpdateGMData::makeRequest(true);
    cMyTrace tr(__PRETTY_FUNCTION__, 0x1d, 0);
    tr("CGmRegistEvent Start!");
}

// ---------------------------------------------------------------------------
// ORIG 0x0811cd76：仅 cMyTrace(__PRETTY_FUNCTION__,0x24,0)
// ("CGmRegistEvent End!")（无 SetEventFlag）。
#include "CGmRegistEvent/EndEvent.inc"
{
    cMyTrace tr(__PRETTY_FUNCTION__, 0x24, 0);
    tr("CGmRegistEvent End!");
}
