// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CCoinRefillEvent/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// CCoinRefillEvent —— 金币补充 repeat 事件实现。
// 全部函数逐段对齐 docs/class_func_reports/CCoinRefillEvent/ 下报告：
//   CCoinRefillEvent.md(0x0832bdf4) / ~CCoinRefillEvent.md(0x0832be2e) /
//   StartEvent.md(0x0832be7c) / StartEvent_0832be82.md(0x0832be82) /
//   EndEvent.md(0x0832c048) / getLastEventTime.md(0x0832c0b8) /
//   getNextEventTime.md(0x0832c148) / registNextRefillTimer.md(0x0832c1fe)。
// [GI-ioinit L21 · P338/P327] 补 <iostream> 恢复 __ioinit ⇒ 发射
// `_GLOBAL__I__ZN16CCoinRefillEventC2Ev`（ORIG 0x832c399）；ctor 已是首强符号。
#include <iostream>

#include "CCoinRefillEvent.h"

#include <cstdio>
#include <cstring>
#include <ctime>

#include "CEnvironment.h"   // G_CEnvironment() / m_serverEnvir.m_fatigueTime（env+0x37c）
#include "CSystemTime.h"
#include "GlobalData.h"     // s_systemTime_ / s_event_manager
#include "TimerQueue.h"     // G_TimerQueue / InsertTimer

#include "CCoinRefillEvent/CCoinRefillEvent.inc"
{
    // ORIG 0x0832bdf4：vptr 由编译器写；memset(this+0xc, -1, 0x14)。
    memset(&m_timeCount, -1, 0x14);
}

#include "CCoinRefillEvent/dtor_CCoinRefillEvent.inc"
{
    // ORIG 0x0832be2e：写 vptr 后调基类析构（编译器生成）。
}

#include "CCoinRefillEvent/StartEvent.inc"
{
    // ORIG 0x0832be7c：纯 ret。
}

#include "CCoinRefillEvent/StartEvent_1.inc"
{
    // [b28d-59 判词] 封 N82（ORIG 0x0832be82）：122/132 条、P167 调用点 6/6 CLEAR
    // （0 缺 0 多）⇒ 语义完成；残差=callee-saved 组（ORIG push edi/esi/ebx）+
    // 物化次数。byte 层封顶。
    SetEventFlag(true);
    memset(&m_timeCount, -1, 0x14);
    m_times[0] = G_CEnvironment()->m_serverEnvir.m_fatigueTime;  // env+0x37c 当前小时槽
    m_times[1] = param.m_duration;               // 低 ushort
    m_times[2] = param.m_field2;                 // 高 ushort
    m_timeCount = 0;
    while ((unsigned int)m_timeCount <= 3u)
    {
        if (m_times[m_timeCount] > 0x17)
        {
            m_times[m_timeCount] = -1;
        }
        if (m_times[m_timeCount] == -1)
        {
            break;
        }
        m_timeCount = m_timeCount + 1;
    }
    for (int i = 1; i <= m_timeCount; i = i + 1)
    {
        int minVal = 0x19;
        int minIdx = 5;
        for (int j = i; j <= m_timeCount; j = j + 1)
        {
            if (m_times[j] < minVal)
            {
                minVal = m_times[j];
                minIdx = j;
            }
        }
        m_times[minIdx] = m_times[i];
        m_times[i] = minVal;
    }
    registNextRefillTimer();
    cMyTrace trace(__PRETTY_FUNCTION__, 0x48, 0);
    trace("Coin Refill Event Start! time(%d) time(%d) time(%d)",
          m_times[0], m_times[1], m_times[2]);
}

#include "CCoinRefillEvent/EndEvent.inc"
{
    // ORIG 0x0832c048。
    SetEventFlag(false);
    memset(&m_timeCount, -1, 0x14);
    cMyTrace trace(__PRETTY_FUNCTION__, 0x4f, 0);
    trace("Coin Refill Event End");
}

#include "CCoinRefillEvent/getLastEventTime.inc"
{
    // [b28d-59] ORIG 0x0832c0b8：改用 m_slots[i] 单数组视角（P235 union 双视角）
    // 复现 ORIG 的 `mov 0xc(%eax,%edx,4)` 折叠形；逻辑槽 i 语义与
    // ORIG `this+i*4+0xc` 逐条一致（旧 b28c-35 真语义 bug 修正保持不变）。
    int curSec = GlobalData::s_systemTime_.getCurSec();
    tm cur;
    localtime_r(reinterpret_cast<time_t*>(&curSec), &cur);
    if (cur.tm_hour < m_slots[1])
    {
        return m_slots[m_timeCount];
    }
    for (int i = 2; i <= m_timeCount; i = i + 1)
    {
        if (cur.tm_hour < m_slots[i])
        {
            return m_slots[i - 1];
        }
    }
    return m_slots[m_timeCount];
}

#include "CCoinRefillEvent/getNextEventTime.inc"
{
    // [b28d-59] ORIG 0x0832c148：m_slots[i] 单数组视角（mov 0xc(%eax,%edx,4)）；
    // 独立 continue 守卫（P133：不得合并 && 短路）——第一循环 `== fatigueTime
    // continue`（sete/jne 形），第二循环 `!= fatigueTime return`（setne/je 形）。
    // 语义：找第一个 != m_fatigueTime 且 > cur.tm_hour 的槽；否则跳过
    // == m_fatigueTime 槽；全失败返回基准槽 m_slots[1]（= m_times[0]）。
    for (int i = 1; i <= m_timeCount; i = i + 1)
    {
        if (m_slots[i] == G_CEnvironment()->m_serverEnvir.m_fatigueTime)
        {
            continue;
        }
        if (cur.tm_hour < m_slots[i])
        {
            return m_slots[i];
        }
    }
    for (int i = 1; i <= m_timeCount; i = i + 1)
    {
        if (m_slots[i] != G_CEnvironment()->m_serverEnvir.m_fatigueTime)
        {
            return m_slots[i];
        }
    }
    return m_slots[1];
}

#include "CCoinRefillEvent/registNextRefillTimer.inc"
{
    // [b28d-59 判词] 封 N82/P231（ORIG 0x0832c1fe）：93/93 条、P167 调用点 9/9 CLEAR
    // ⇒ 语义完成；残差=帧 0x94 vs 0xa8 + ORIG 用 %ebx 跨调用保活（callee-saved）。
    // byte 层封顶。
    int curSec = GlobalData::s_systemTime_.getCurSec();
    tm cur;
    localtime_r(reinterpret_cast<time_t*>(&curSec), &cur);
    tm next = cur;
    next.tm_hour = getNextEventTime(cur);
    if (next.tm_hour < cur.tm_hour)
    {
        next.tm_mday = next.tm_mday + 1;
    }
    next.tm_min = 0;
    next.tm_sec = 0;
    time_t nextSec = mktime(&next);
    if (nextSec - static_cast<time_t>(curSec) >= 0)
    {
        int delay = static_cast<int>(nextSec) - curSec;
        G_TimerQueue()->InsertTimer(static_cast<TimerEntry::OBJ_TYPE>(2), 0,
                                    static_cast<TIMER_MESSAGE>(0x65), delay, 0, 0);
        char* timeStr = ctime(&nextSec);
        cMyTrace trace(__PRETTY_FUNCTION__, 0x88, 0);
        trace("Next coin refill time is %s", timeStr);
    }
}
