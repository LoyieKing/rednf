// df_game_r Game/ CEventBase 实现（独立 TU，从 GameStubs.cpp 拆出）。
// 声明以 CGuildServerProxy.h 为准。
// 依据 docs/class_func_reports/CEventBase.md + ORIG elf 反汇编：
//   ctor 0x0810ae2c（调用 stADDINFO ctor 0x0810af10，+0x08 flag 置 0）
//   ~ctor 0x0810ae52 / StartEvent(Word_Param) 0x0810af26（空）
//   StartAction 0x080c84c4（空）/ AppendInfo 0x080c84ca（空）
//   GetAddInfo 0x080c84d0（返回 +0x04 的 4 字节）
//   SetEventFlag 0x080c84e2（写 +0x08）/ IsEventing 0x080c84fa（读 +0x08）
//   changeCharacName/dailyresetData/deliveryStart/deliveryEnd/delivery 空。
#include "CGuildServerProxy.h"

// ORIG 0x0810af10：两个 ushort 均置 0xffff。
stADDINFO::stADDINFO()
{
    m_info0 = 0xffff;
    m_info1 = 0xffff;
}

CEventBase::CEventBase()
    : m_addInfo()
{
    // ORIG：*(vptr) = &vtable_08b4bae8（编译器自动）；
    // stADDINFO ctor 0x0810af10：两个 ushort 置 0xffff；+0x08 flag 置 0。
    m_isEventing = false;
}

CEventBase::~CEventBase()
{
    // ORIG 仅恢复基类 vptr；删除由 deleting destructor 负责。
}

void CEventBase::StartEvent(Word_Param) {}
void CEventBase::StartAction() {}
void CEventBase::AppendInfo(PacketGuard&) {}

int CEventBase::GetAddInfo()
{
    // ORIG 0x080c84d0：直接读取 this+0x04 的 stADDINFO 四字节值。
    return *reinterpret_cast<const int*>(
        reinterpret_cast<const char*>(this) + sizeof(void*));
}

void CEventBase::changeCharacName(CUser*, int, char*, int) {}
void CEventBase::dailyresetData() {}
void CEventBase::deliveryStart(CDelivery*) {}
void CEventBase::deliveryEnd(CDelivery*) {}
void CEventBase::delivery(DeliveryMsg&) {}

bool CEventBase::IsEventing(CUser*) const
{
    // ORIG 0x080c84fa：返回 +0x08 的 flag 字节。
    return m_isEventing;
}

void CEventBase::SetEventFlag(bool flag)
{
    // ORIG 0x080c84e2：写 +0x08 的 flag 字节。
    m_isEventing = flag;
}
