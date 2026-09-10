// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CEventStayTime/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// [GI-ioinit L21 · P338/P327] 补 <iostream> 恢复 __ioinit ⇒ 发射
// `_GLOBAL__I__ZN14CEventStayTime12kRewardItem_E`（ORIG 0x816c118）；
// 类静态 kRewardItem_ 定义（R 型、外部链接）已是本 TU 首强符号。
#include <iostream>

#include "CEventStayTime.h"
#include "CStreamGuard.h"
#include "GlobalData.h"
#include "LogManager.h"
#include "MsgQueueMgr.h"
#include "MoreTypes.h"
#include "StreamPool.h"
#include "TimerQueue.h"
#include "TaiwanInternal.h"
#include "GameTypes.h"

// ORIG 类静态 _ZN14CEventStayTime12kRewardItem_E（R 0x08b800a0，4 字节只读，
// 值 0x2920908d）。类内声明 + 类外定义（无 L，外部链接）。
const int CEventStayTime::kRewardItem_ = 0x2920908d;

// ORIG 0x816c066：仅基类构造 + vptr 置位 + movb $1,0x9（m_firstReset=true）。
// m_rewardStayTime/m_intervalStayTime 不清零（ORIG 无对应 movl）。
#include "CEventStayTime/CEventStayTime.inc"
    : CEventBase(), m_firstReset(true)
{
}

#include "CEventStayTime/dtor_CEventStayTime.inc"
{
}

// ORIG 0x816bda0：SetEventFlag(true)；实参（interval→esi、reward→ebx）在
// cMyTrace 构造前物化（P37 链式临时同形）；__PRETTY_FUNCTION__ 符号压参。
#include "CEventStayTime/StartEvent.inc"
{
    SetEventFlag(true);
    cMyTrace(__PRETTY_FUNCTION__, 10, 0)(
        "[Taiwan, StayEvent] Start! StayTime:%dMin, CheckTime:%dMin",
        m_rewardStayTime, m_intervalStayTime);
    RegistNextTimer();
}

// ORIG 0x816be60：logFormat(1, file, __PRETTY_FUNCTION__, 0x1b, msg) 先行，
// 再清零 0xc/0x10，最后 SetEventFlag(false)。
#include "CEventStayTime/EndEvent.inc"
{
    LogManager::logFormat(1, "localtaiwan/Event/EventStayTimeCharacter.cpp",
                          __PRETTY_FUNCTION__, 0x1b, "[CEventStayTime] End Event.");
    m_rewardStayTime = 0;
    m_intervalStayTime = 0;
    SetEventFlag(false);
}

// ORIG 0x816be18：(param 低字)*60 → +0xc、(param 高字)*60 → +0x10，
// 然后经 vptr+0x8 虚调 StartEvent()。
#include "CEventStayTime/StartEvent_1.inc"
{
    m_rewardStayTime = static_cast<unsigned int>(param.m_duration) * 60u;
    m_intervalStayTime = static_cast<unsigned int>(param.m_field2) * 60u;
    this->StartEvent();
}

// ORIG 0x816bf6a：IsEventing(0) 与 IsFirstReset() 双 early-return 后
// Acquire → CStreamGuard(stream,true) → internal_stream(7,-1) →
// operator-> 后 GetInBuffer 判空 → put(DB_Q)。无任何空指针检查。
#include "CEventStayTime/dailyresetData.inc"
{
    if (!IsEventing(0))
        return;
    if (IsFirstReset())
        return;
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
        "localtaiwan/Event/EventStayTimeCharacter.cpp", 0x4a), true);
    Taiwan::internal_stream(guard, static_cast<TaiwanInternalPack::T>(7), -1);
    Taiwan::SigResetStayTimeEvent* sig =
        guard->GetInBuffer<Taiwan::SigResetStayTimeEvent>();
    if (sig != 0)
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}

// ORIG 0x816befa：!IsEventing(0) early-return；G_TimerQueue() 内联实参
// （P24，q 不落局部）；无 queue 空检查。
#include "CEventStayTime/RegistNextTimer.inc"
{
    if (!IsEventing(0))
        return;
    // [b28-L14] 残差① 封顶 N7：IsEventing 虚调用 ORIG `mov (%eax),%edx;
    // mov this,%eax; call *%edx` vs 4.4 恒把间接 call 目标钉 %eax
    // （探针 /tmp/l14p/virt2.cpp f/g/h 四形实测）。
    // 残差② 已翻正：ORIG 0x816befa `mov 0x10(%eax),%ebx` 单段直落 callee-saved
    // 并跨 G_TimerQueue() 保活——P226 语句表达式形无效，但**裸 register（无
    // asm 绑定，P0 合规）**使 4.4 强度归约为同一条 `mov disp(%eax),%ebx`
    // （探针 /tmp/l14p/plreg.cpp a2/a3/a5：a4 普通局部两段、a5 裸 register 单段）。
    register unsigned int interval = m_intervalStayTime;
    G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_2, 0,
                                static_cast<TIMER_MESSAGE>(0xad),
                                interval, 0, 0);
}

// ORIG 0x816bed4：真分支清零返回 1；假分支读回字段原值（movzbl 0x9）。
#include "CEventStayTime/IsFirstReset.inc"
{
    if (m_firstReset) {
        m_firstReset = false;
        return true;
    }
    return m_firstReset;
}

#include "CEventStayTime/getRewardStayTime.inc"
{
    return m_rewardStayTime;
}

#include "CEventStayTime/getIntervalStayTime.inc"
{
    return m_intervalStayTime;
}
