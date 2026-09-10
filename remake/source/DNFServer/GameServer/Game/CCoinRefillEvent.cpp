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
    // [b28d-59 判词] （ORIG 0x0832be82）：122/132 条、P167 调用点 6/6 CLEAR。
    // [L6S2b 修 · 视角 + 语义] ORIG 全部槽访问走 m_slots 单数组视角（基址 +0xc）：
    //   ①while 段 `mov 0xc(%eax),%eax; lea 0x1(%eax),%edx; mov 0xc(%eax,%edx,4)`
    //     = `m_slots[m_timeCount + 1]`（旧译 `m_times[m_timeCount]` 走 +0x10 基址，
    //     地址虽同但少一条 lea）；
    //   ②for 段 `mov 0xc(%eax,%edx,4)`（0x0832bf78/bf87，edx=j=j..m_timeCount，j 从
    //     i=1 起）= `m_slots[j]`；旧译 `m_times[j]` 基址 +0x10 ⇒ **下标整体错 1 且
    //     j=m_timeCount=4 时读到 +0x20（union 只到 +0x1f）= 越界读**（运行期真 bug）。
    //   交换段同理 `m_slots[minIdx]`/`m_slots[i]`（0x0832bfb1/bfc5）。
    // [L6S2b 修 · P227] ORIG 三个 trace 实参在 cMyTrace ctor **之前**右起求值并驻
    //   %edi/%esi/%ebx 保活（`mov 0x18(%eax),%edi; mov 0x14(%eax),%esi;
    //   mov 0x10(%eax),%ebx` @0x0832bfef..bffb，prologue push edi/esi/ebx）⇒ 具名
    //   两语句形坍缩为 ctor 后 ecx/edx/eax 装载；改全表达式临时链即复现。
    SetEventFlag(true);
    memset(&m_timeCount, -1, 0x14);
    m_times[0] = G_CEnvironment()->m_serverEnvir.m_fatigueTime;  // env+0x37c 当前小时槽
    m_times[1] = param.m_duration;               // 低 ushort
    m_times[2] = param.m_field2;                 // 高 ushort
    m_timeCount = 0;
    while ((unsigned int)m_timeCount <= 3u)
    {
        if (m_slots[m_timeCount + 1] > 0x17)
        {
            m_slots[m_timeCount + 1] = -1;
        }
        if (m_slots[m_timeCount + 1] == -1)
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
            if (m_slots[j] < minVal)
            {
                minVal = m_slots[j];
                minIdx = j;
            }
        }
        m_slots[minIdx] = m_slots[i];
        m_slots[i] = minVal;
    }
    registNextRefillTimer();
    cMyTrace(__PRETTY_FUNCTION__, 0x48, 0)(
        "Coin Refill Event Start! time(%d) time(%d) time(%d)",
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
    // [b28d-59 判词] （ORIG 0x0832c1fe）：93/93 条、P167 调用点 9/9 CLEAR。
    // [L6S2b 形修正 · P0/P183/P231] ORIG 该函数有两处「值无槽往返、驻 callee-saved
    //   %ebx 跨 call 保活」：① `mov -0x20(%ebp),%edx; mov -0x1c(%ebp),%eax;
    //   mov %edx,%ebx; sub %eax,%ebx; call G_TimerQueue; … mov %ebx,0x10(%esp)`
    //   （0x0832c2c9..2e2，delay 实参）；② `call ctime; mov %eax,%ebx; … call
    //   cMyTrace::C1; mov %ebx,0x8(%esp)`（0x0832c311/336，ctime 结果）。
    //   我方形为槽（`mov %eax,-0x10/-0xc(%ebp)` + 回读）且无 `push %ebx`（帧 0xa8 vs 0x94）。
    //   修法：①删具名 `int delay`、表达式内联回实参位；②`char*` 加 `register`
    //   存储类（裸 register 属 P231/P307 命中形：值跨 callee call 保活 + 4.4 不自
    //   升格 + ORIG 目标为 callee-saved，非 asm 绑定故 P0 合规）。
    //   探针 /tmp/L6S2_probe/pr_tq.cpp（g_named 槽/无 push vs g_inline ebx+push）与
    //   pr_ts.cpp（h_plain 槽 vs h_reg ebx+push，h_inline 会改 ctime 调用序故不可用）
    //   均与 ORIG 逐形一致。
    // [M4S2 · P339 全头部声明 + 声明序按 ORIG 槽深→浅] ORIG 0x0832c1fe 槽图
    //   cur=-0x78 / next=-0x4c / nextSec=-0x20 / curSec=-0x1c / trace=-0x18，
    //   即三件 4B/16B 的槽**自低向高按声明序**紧密排布（next 尾 -0x20 → nextSec
    //   → curSec → trace）。旧译把 curSec/nextSec 写成带初值的语句级声明、trace
    //   在内层块 ⇒ 我方槽序反为 trace=-0x20 / nextSec=-0x10 / curSec=-0xc
    //   （NEAR 残差唯一来源）。改为头部纯声明 + 语句级赋值后逐槽对齐。
    tm cur;
    tm next;
    time_t nextSec;
    int curSec;
    curSec = GlobalData::s_systemTime_.getCurSec();
    localtime_r(reinterpret_cast<time_t*>(&curSec), &cur);
    next = cur;
    next.tm_hour = getNextEventTime(cur);
    if (next.tm_hour < cur.tm_hour)
    {
        next.tm_mday = next.tm_mday + 1;
    }
    next.tm_min = 0;
    next.tm_sec = 0;
    nextSec = mktime(&next);
    if (nextSec - static_cast<time_t>(curSec) >= 0)
    {
        G_TimerQueue()->InsertTimer(static_cast<TimerEntry::OBJ_TYPE>(2), 0,
                                    static_cast<TIMER_MESSAGE>(0x65),
                                    static_cast<int>(nextSec) - curSec, 0, 0);
        register char* timeStr = ctime(&nextSec);
        cMyTrace trace(__PRETTY_FUNCTION__, 0x88, 0);
        trace("Next coin refill time is %s", timeStr);
    }
    else
    {
        // [M4S2 · P223 尾岛] ORIG 0x0832c2bd..0x0832c350 形为
        //   `js +0x151`（落 nop）→ 体内三条语句 → `jmp +0x152` → `+0x151: nop`
        //   → epilogue，即 then 块尾有跳过 else 岛的 jmp、失败落点另有空标签岛。
        //   旧译裸 if（无 else）产 `js <epilogue>` 直连，缺这 2 条（gap +2 来源）。
        //   探针 /tmp/m4s2_probe/pr_tail.cpp：f6(`if (c>=0){g();h();} else {return;}`)
        //   产 `js L_else; call; call; jmp L_end; L_else: nop; L_end:`（= ORIG 逐条），
        //   f1..f5（裸 if/空 else/空语句）均不产 jmp+nop。语义不变（void 尾 return）。
        return;
    }
    // [M4S2 R2 复核 · 尾岛已修；残差 = N95/N126 槽映射族]
    //   gap 已 +2→0（原缺 `jmp +0x152`/`nop +0x151` 两条，见上）。剩余唯一差异是
    //   三件局部槽的**排列**：ORIG 0x0832c208/0x0832c2ac/0x0832c32b 槽序（低→高）
    //   = nextSec(-0x20) < curSec(-0x1c) < trace(-0x18)；我方 = trace(-0x20) <
    //   nextSec(-0x10) < curSec(-0xc)（本形）。两形均实测：
    //   ① 语句级带初值声明（`int curSec = ...;` / `time_t nextSec = ...;`）⇒
    //      curSec(-0xc) < nextSec(-0x10) < trace(-0x20)；
    //   ② P339 全头部纯声明 + 语句级赋值（本形）⇒ nextSec(-0xc) < curSec(-0x10)
    //      < trace(-0x20)：4B 槽序已对齐，但 16B 块内非 POD 对象 trace 恒落
    //      `next` 之下的第一个空位 -0x20，不再上移。
    //   ⇒ 4.4 对块内非 POD 对象的槽分配时机早于外层标量，ORIG 的 trace 位于标量
    //      之上非源码可及 ⇒ 保留本形（DIFF→NEAR，指令序列除槽号外逐条一致）。
}
