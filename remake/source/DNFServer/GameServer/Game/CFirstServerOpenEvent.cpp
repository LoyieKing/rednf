// df_game_r CFirstServerOpenEvent 实现（OrigMiscEvt 批补齐）。
// 声明见 CFirstServerOpenEvent.h。逐函数依据
// docs/class_func_reports/CFirstServerOpenEvent/ 报告转写：
//   C1 0x0811ca78  D1 0x0811ca94  D0 0x0811cac4
//   StartEvent() 0x0811cae2  StartEvent(Word_Param) 0x0811cb34
//   EndEvent 0x0811cb86  IsEventing 0x0811cbd8
// vtable 0x08b530e0 = 基类 14 槽 + 派生新增槽 15（IsEventing 非 const
// 重载，未覆盖基类 const IsEventing）；无派生数据字段（sizeof 0xc）。
// 追踪用 __FUNCTION__（_ZZN21CFirstServerOpenEvent*E12__FUNCTION__）。
// [GI-ioinit L21 · P338/P327] 补 <iostream> 恢复 __ioinit ⇒ 发射
// `_GLOBAL__I__ZN21CFirstServerOpenEventC2Ev`（ORIG 0x811cc46）；ctor 已是首强符号。
#include <iostream>

#include "CFirstServerOpenEvent.h"
#include "GameTypes.h"   // cMyTrace

// ORIG C1 0x0811ca78：仅基类构造 + vptr（无派生字段）。
CFirstServerOpenEvent::CFirstServerOpenEvent()
{
}
// ORIG D1 0x0811ca94：用户空析构（编译器生成 vptr+基类析构）。
CFirstServerOpenEvent::~CFirstServerOpenEvent() {}

// ORIG 0x0811cbd8：显式调基类 IsEventing（call 0x080c84fa；实参 0）。
// 返回形态对齐 ORIG xor $1/test/je/mov0/mov1（P22/P59：bool 取反 guard）。
bool CFirstServerOpenEvent::IsEventing(CUser* /*user*/)
{
    if (!CEventBase::IsEventing((CUser*)0))
        return false;
    return true;
}
// ORIG 0x0811cae2：置标志；trace 行号 32。
void CFirstServerOpenEvent::StartEvent()
{
    SetEventFlag(true);
    cMyTrace trace(__FUNCTION__, 32, 0);
    trace("First Server Open Event Start!");
}
// ORIG 0x0811cb34：忽略参数；置标志；trace 行号 46。
// 报告反编译：StartEvent(Word_Param) 与无参版完全相同（参数未使用），
// 汇编仅 SetEventFlag + cMyTrace 两段，无字段写入（该类无派生字段）。
// 与 StartEvent() 的唯一差异是行号（0x2e vs 0x20）与 __FUNCTION__ 符号。
// 参数按值传入（Word_Param 4 字节），无成员写入。
// 行号 0x2e=46（ORIG movl $0x2e,0x8(%esp)）。
// param 未参与任何运算（ORIG 无 movzwl 读参）。
// Word_Param 按值传参（4 字节），汇编仅 SetEventFlag 段后即 trace。
void CFirstServerOpenEvent::StartEvent(Word_Param param)
{
    SetEventFlag(true);
    cMyTrace trace(__FUNCTION__, 46, 0);
    trace("First Server Open Event Start!");
}
void CFirstServerOpenEvent::EndEvent()
{
    SetEventFlag(false);
    cMyTrace trace(__FUNCTION__, 52, 0);
    trace("First Server Open Event End!");
}
