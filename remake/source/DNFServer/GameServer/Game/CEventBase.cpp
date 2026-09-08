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

// [b28d-30] 复核 NEAR：ORIG 帧 0x18 vs 4.4 0x14（P190 附注：带 call 帧 sub 恒 16k+4
// 量化，0x18 非量化值不可凑；dead int×4=0x14、+double 跳 0x24）⇒ N82/P190 封顶。
CEventBase::CEventBase()
    : m_addInfo()
{
    // ORIG：*(vptr) = &vtable_08b4bae0（编译器自动）；
    // stADDINFO ctor 0x0810af10：两个 ushort 置 0xffff；+0x08 flag 置 0。
    // [b28 判词·N·帧阶梯不可达] ORIG 帧 0x18 vs 4.4 0x14：探针 /tmp/l2_p8.cpp
    // 实测 4.4 本形帧仅 {0x14,0x24} 两档——4 dead int=0x14、+double/第5 int 跳
    // 0x24（P190 double 撑帧此 TU 过冲，P31 int 垫无中间值）；stADDINFO ctor 为
    // 他 TU 外联（P210 同 TU 继承律不适用）。唯一真差 = sub 立即数 1 行，N82/P182 族。
    int d1, d2, d3, d4;
    m_isEventing = false;
}

CEventBase::~CEventBase()
{
    // ORIG 仅恢复基类 vptr；删除由 deleting destructor 负责。
}

void CEventBase::StartEvent(Word_Param) {}
void CEventBase::StartAction() {}
void CEventBase::AppendInfo(PacketGuard&) {}

stADDINFO CEventBase::GetAddInfo()
{
    // ORIG 0x080c84d0：返回 this+0x04 的 stADDINFO（4 字节非 POD → gcc 4.4
    // sret：隐藏 out 指针 + this 栈参，ret $4；本定义编译产物与 ORIG 逐条同形，
    // 2026-08-28 OrigEventLeaf8f 由 int 修正为 stADDINFO，mangled 不变 Ev）。
    return m_addInfo;
}

void CEventBase::changeCharacName(CUser*, int, char*, int) {}
void CEventBase::dailyresetData() {}      // ORIG 0x0810ae9c（0x6 字节空体）
void CEventBase::deliveryStart(CDelivery*) {}  // ORIG 0x0810aea2（空体）
void CEventBase::deliveryEnd(CDelivery*) {}
void CEventBase::delivery(DeliveryMsg&) {}

bool CEventBase::IsEventing(CUser*) const
{
    // ORIG 0x080c84fa：movzbl 直返 +0x08 的 flag 字节。
    return m_isEventing;
}

void CEventBase::SetEventFlag(bool flag)
{
    // ORIG 0x080c84e2：写 +0x08 的 flag 字节。
    m_isEventing = flag;
}
