// CCharacterDayEvent —— 角色生日事件实现。
// 函数逐段对齐 docs/class_func_reports/CCharacterDayEvent/ 下报告：
//   CCharacterDayEvent.md(C1 0x0810a4c8) / ~CCharacterDayEvent.md(D1 0x0810a4ee) /
//   ~CCharacterDayEvent_0810a51e.md(D0 0x0810a51e) / StartEvent.md(0x0810a53c) /
//   StartEvent_0810a58e.md(0x0810a58e) / EndEvent.md(0x0810a5ec)。
// GetEventJob(0x08697886) 无独立报告（nm 定址 objdump：movzwl 0xa 直返）。
#include "CCharacterDayEvent.h"

#include "GameTypes.h"   // cMyTrace

CCharacterDayEvent::CCharacterDayEvent()
{
    // ORIG 0x0810a4c8：基类 CEventBase ctor + 写 vptr（编译器生成）；
    // movw $0xffff,0xa(%eax) 置 m_eventJob（-1 位模式）。
    m_eventJob = -1;
}

CCharacterDayEvent::~CCharacterDayEvent()
{
    // ORIG 0x0810a4ee：写 vptr 后调 CEventBase::~CEventBase（编译器生成）。
}

void CCharacterDayEvent::StartEvent()
{
    // ORIG 0x0810a53c：SetEventFlag(true) + cMyTrace(__FUNCTION__,0x17,0)(...)。
    SetEventFlag(true);
    cMyTrace(__FUNCTION__, 0x17, 0)("Character Day Event Start!");
}

void CCharacterDayEvent::StartEvent(Word_Param param)
{
    // ORIG 0x0810a58e：SetEventFlag(true)；movzwl 0xc(%ebp),%eax → mov %eax,%edx
    // → mov %dx,0xa 写 m_eventJob（unsigned ushort 目标，零扩展直写 16 位）；
    // cMyTrace(__FUNCTION__,0x1f,0)(...)。残差 N18a：4.4 恒折
    // movzwl 0xc(%ebp),%edx 单段（两段中转不可复现）。
    SetEventFlag(true);
    m_eventJob = param.m_duration;
    cMyTrace(__FUNCTION__, 0x1f, 0)("Character Day Event Start!");
}

void CCharacterDayEvent::EndEvent()
{
    // ORIG 0x0810a5ec：SetEventFlag(false) + cMyTrace(__FUNCTION__,0x25,0)(...)。
    SetEventFlag(false);
    cMyTrace(__FUNCTION__, 0x25, 0)("Character Day Event End!");
}

unsigned short CCharacterDayEvent::GetEventJob()
{
    // ORIG 0x08697886：movzwl 0xa(%eax),%eax 直返（unsigned short 返回无规范化）。
    return m_eventJob;
}
