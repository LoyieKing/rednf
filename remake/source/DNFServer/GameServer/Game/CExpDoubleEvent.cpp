// df_game_r Game/ CExpDoubleEvent 实现（批3：从 GameStubs.cpp/CExpDoubleEvent.cpp 迁移）。
// 依据 docs/class_func_reports/CExpDoubleEvent/*.md + ORIG elf 反汇编：
//   ctor 0x849ac44：CEventBase() + m_factor=100
//   D1 0x849ac6a：恢复 vptr 后调 CEventBase::~CEventBase()；D0 0x849ac9a 再 operator delete
//   StartEvent 0x849acb8 / EndEvent 0x849ad72 / StartEvent(Word_Param) 0x849ad14
//   GetExpFactor 0x0814aad8：return +0x0c
// [GI-ioinit L21 · P338/P327] 补 <iostream> 恢复 __ioinit ⇒ 发射
// `_GLOBAL__I__ZN15CExpDoubleEventC2Ev`（ORIG 0x849ae0d）；ctor 已是首强符号。
#include <iostream>

#include "CExpDoubleEvent.h"

#include "cMyTraceNoop.h"

CExpDoubleEvent::CExpDoubleEvent()
{
    // ORIG：CEventBase ctor 置基类 vptr 与 stADDINFO/+0x08 flag；本类 vptr 由编译器设置。
    m_factor = 100;
}

CExpDoubleEvent::~CExpDoubleEvent()
{
    // ORIG D1：恢复本类 vptr，再调 CEventBase::~CEventBase()（空）。
}

void CExpDoubleEvent::StartEvent()
{
    SetEventFlag(true);
    m_factor = 100;
    cMyTrace tr(__FUNCTION__, 0x13, 0);
    tr("Exp Double Event Start!");
}

void CExpDoubleEvent::StartEvent(Word_Param param)
{
    SetEventFlag(true);
    m_factor = (unsigned int)param.m_duration;
    cMyTrace tr(__FUNCTION__, 0x1a, 0);
    tr("Exp Double Event Start!");
}

void CExpDoubleEvent::EndEvent()
{
    SetEventFlag(false);
    m_factor = 100;
    cMyTrace tr(__FUNCTION__, 0x21, 0);
    tr("Exp Double Event End!");
}

int CExpDoubleEvent::GetExpFactor()
{
    // ORIG 0x0814aad8：return *(int*)(this+0x0c)。
    return m_factor;
}
