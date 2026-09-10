// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CBloodDungeonRewardEvents/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================

#include "CBloodDungeonRewardFirstEvent.h"
#include "CBloodDungeonRewardSecondEvent.h"

// ---------------------------------------------------------------------------
// CBloodDungeonRewardFirstEvent
// ---------------------------------------------------------------------------

#include "CBloodDungeonRewardEvents/CBloodDungeonRewardFirstEvent.inc"
{
    // ORIG C1 0x0811bdde：CEventBase() 后 vptr=0x8b52208（vtable+8），m_gold(+0xc)=0。
    m_gold = 0;
}

#include "CBloodDungeonRewardEvents/dtor_CBloodDungeonRewardFirstEvent.inc"
{
    // ORIG D1 0x0811be04：恢复本类 vptr 后调 CEventBase::~CEventBase()（空）。
}

#include "CBloodDungeonRewardEvents/StartEvent.inc"
{
    // ORIG 0x0811bd12：SetEventFlag(true) + cMyTrace(__PRETTY_FUNCTION__, 0x10, 0)。
    SetEventFlag(true);
    cMyTrace tr(__PRETTY_FUNCTION__, 0x10, 0);
    tr("CBloodDungeonRewardFirstEvent Start!");
}

#include "CBloodDungeonRewardEvents/StartEvent_1.inc"
{
    // [N7 封顶][b28c-33 复核 16/16] 虚调用 callee 寄存器 %edx(ORIG)↔%eax(4.4) 互换。
    // ORIG 0x0811bd64：movzwl 零扩展 param 低 ushort 写 +0x0c，再经 vptr 槽 2 虚调
    // StartEvent()。成员内未限定调用即为虚分派，与 ORIG call *vptr+8 一致。
    m_gold = (unsigned short)param.m_duration;
    StartEvent();
}

#include "CBloodDungeonRewardEvents/EndEvent.inc"
{
    // ORIG 0x0811bd8c：SetEventFlag(false) + cMyTrace(__PRETTY_FUNCTION__, 0x2d, 0)。
    SetEventFlag(false);
    cMyTrace tr(__PRETTY_FUNCTION__, 0x2d, 0);
    tr("CBloodDungeonRewardFirstEvent End!");
}

#include "CBloodDungeonRewardEvents/GetGold.inc"
{
    // ORIG W 0x085bff1a：return *(int*)(this+0xc)。
    return m_gold;
}

// ---------------------------------------------------------------------------
// CBloodDungeonRewardSecondEvent
// ---------------------------------------------------------------------------

#include "CBloodDungeonRewardEvents/CBloodDungeonRewardSecondEvent.inc"
{
    // ORIG C1 0x0811bf1e：CEventBase() 后 vptr=0x8b521c8（vtable+8），m_gold(+0xc)=0。
    m_gold = 0;
}

#include "CBloodDungeonRewardEvents/dtor_CBloodDungeonRewardSecondEvent.inc"
{
    // ORIG D1 0x0811bf44：恢复本类 vptr 后调 CEventBase::~CEventBase()（空）。
}

#include "CBloodDungeonRewardEvents/StartEvent_2.inc"
{
    // ORIG 0x0811be52：SetEventFlag(true) + cMyTrace(__PRETTY_FUNCTION__, 0x10, 0)。
    SetEventFlag(true);
    cMyTrace tr(__PRETTY_FUNCTION__, 0x10, 0);
    tr("CBloodDungeonRewardSecondEvent Start!");
}

#include "CBloodDungeonRewardEvents/StartEvent_3.inc"
{
    // [N7 封顶][b28c-33 复核 16/16] 虚调用 callee 寄存器 %edx(ORIG)↔%eax(4.4) 互换。
    // ORIG 0x0811bea4：movzwl 零扩展 param 低 ushort 写 +0x0c，再经 vptr 槽 2 虚调
    // StartEvent()。
    m_gold = (unsigned short)param.m_duration;
    StartEvent();
}

#include "CBloodDungeonRewardEvents/EndEvent_1.inc"
{
    // ORIG 0x0811becc：SetEventFlag(false) + cMyTrace(__PRETTY_FUNCTION__, 0x1b, 0)。
    SetEventFlag(false);
    cMyTrace tr(__PRETTY_FUNCTION__, 0x1b, 0);
    tr("CBloodDungeonRewardSecondEvent End!");
}

#include "CBloodDungeonRewardEvents/GetGold_1.inc"
{
    // ORIG W 0x085bff26：return *(int*)(this+0xc)。
    return m_gold;
}
