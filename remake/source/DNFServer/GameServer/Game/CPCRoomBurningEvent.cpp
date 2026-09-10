// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CPCRoomBurningEvent/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// df_game_r Game/ CPCRoomBurningEvent 实现。
// 依据 docs/class_func_reports/CPCRoomBurningEvent/*.md + ORIG elf 反汇编：
//   ctor 0x082671dc：CEventBase() + m_factor=100
//   D1 0x08267202：恢复 vptr 后调 CEventBase::~CEventBase()；D0 0x08267232 再 operator delete
//   StartEvent() 0x08267250 / StartEvent(Word_Param) 0x082672ac /
//   EndEvent() 0x0826730a：SetEventFlag + m_factor 写入 + cMyTrace 文本
//   IsEventing 0x08267366：基类判活且 user 非空时返回 isAffectedPremium(user,0xc)
//   GetExpFactor 0x0814aae4 W：return +0x0c
#include "CPCRoomBurningEvent.h"

#include "CUser.h"
#include "cMyTraceNoop.h"

#include "CPCRoomBurningEvent/CPCRoomBurningEvent.inc"
{
    // ORIG：CEventBase ctor 置基类 vptr 与 stADDINFO/+0x08 flag；本类 vptr 由编译器设置。
    m_factor = 100;
}

#include "CPCRoomBurningEvent/dtor_CPCRoomBurningEvent.inc"
{
    // ORIG D1：恢复本类 vptr，再调 CEventBase::~CEventBase()（空）。
}

#include "CPCRoomBurningEvent/StartEvent.inc"
{
    SetEventFlag(true);
    m_factor = 100;
    cMyTrace tr(__FUNCTION__, 0x14, 0);
    tr("PC Room Burning Event Start!");
}

#include "CPCRoomBurningEvent/StartEvent_1.inc"
{
    SetEventFlag(true);
    m_factor = (int)(unsigned int)param.m_duration;
    cMyTrace tr(__FUNCTION__, 0x1c, 0);
    tr("PC Room Burning Event Start!");
}

#include "CPCRoomBurningEvent/EndEvent.inc"
{
    SetEventFlag(false);
    m_factor = 100;
    cMyTrace tr(__FUNCTION__, 0x24, 0);
    tr("PC Room Burning Event End!");
}

#include "CPCRoomBurningEvent/IsEventing.inc"
{
    // ORIG 0x08267366：先以 NULL 调基类 CEventBase::IsEventing 判总开关，
    // 再单独检查 user 非空；两者皆真时返回 isAffectedPremium(user, 0xc)。
    if (!CEventBase::IsEventing(0) || user == 0)
        return false;
    if (!user->isAffectedPremium((ENUM_PREMIUM_TYPE)0xc))
        return false;
    return true;
}

#include "CPCRoomBurningEvent/GetExpFactor.inc"
{
    // ORIG 0x0814aae4：return *(int*)(this+0x0c)。
    return m_factor;
}
