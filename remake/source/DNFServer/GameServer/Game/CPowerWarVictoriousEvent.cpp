// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CPowerWarVictoriousEvent/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// df_game_r 力战胜利事件 CPowerWarVictoriousEvent 实现。
// 报告：docs/class_func_reports/CPowerWarVictoriousEvent/ 全部 6 个方法
// （CPowerWarVictoriousEvent / ~CPowerWarVictoriousEvent / ~CPowerWarVictoriousEvent_08268968 /
//  StartEvent / StartEvent_08268a68 / EndEvent）。
// [GI-ioinit L21 · P338/P327] 补 <iostream> 恢复 __ioinit ⇒ 发射
// `_GLOBAL__I__ZN24CPowerWarVictoriousEventC2Ev`（ORIG 0x8268c0b）；ctor 已是首强符号。
#include <iostream>

#include "CPowerWarVictoriousEvent.h"

#include "CEnvironment.h"          // G_CEnvironment / m_serverEnvir.m_channelNo
#include "CDataManager.h"          // G_CDataManager / m_powerParameter.m_victoriousRewardTime
#include "TimerQueue.h"            // G_TimerQueue / TimerEntry / TIMER_MESSAGE
#include "GameTypes.h"             // cMyTrace

// ---------------------------------------------------------------------------
// ORIG 0x0826891c：仅基类构造 + vptr 置位（无自有成员初始化）。
#include "CPowerWarVictoriousEvent/CPowerWarVictoriousEvent.inc"
{
}

// ORIG 0x08268938 / 0x08268968：D1 重置 vptr 后调 CEventBase::~CEventBase；
// D0 调 D1 后 operator delete。空体 + 虚析构由编译器生成同形。
#include "CPowerWarVictoriousEvent/dtor_CPowerWarVictoriousEvent.inc"
{
}

// ---------------------------------------------------------------------------
// ORIG 0x08268986：IsEventing(0) 为假时置事件标志、挂 0x8b 定时器
// （时长 = m_victoriousRewardTime × 0x3c 分钟）并打日志。
#include "CPowerWarVictoriousEvent/StartEvent.inc"
{
    // [N7 封顶] IsEventing 虚调用 callee 寄存器 %edx(ORIG)↔%eax(4.4) 互换。
    if (!IsEventing(0))
    {
        SetEventFlag(true);
        int rewardTime = G_CDataManager()->m_powerParameter.m_victoriousRewardTime * 0x3c;
        G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_2, 0, (TIMER_MESSAGE)0x8b,
                                    rewardTime, 0, 0);
        register int serverNo = G_CEnvironment()->m_serverEnvir.m_channelNo;
        cMyTrace tr(__PRETTY_FUNCTION__, 0x22, 0);
        tr("Power War Victorious Event Start! server(%d)", serverNo);
    }
}

// ---------------------------------------------------------------------------
// ORIG 0x08268a68：同 StartEvent()（ORIG 未写 m_param）。
#include "CPowerWarVictoriousEvent/StartEvent_1.inc"
{
    // [N7 封顶] ORIG 虚调用 funcptr→%edx、this→%eax；4.4 恒 funcptr→%eax
    // （探针 /tmp/nl24p p1/p2 三源形全败），寄存器名差封顶。
    if (!IsEventing(0))
    {
        SetEventFlag(true);
        int rewardTime = G_CDataManager()->m_powerParameter.m_victoriousRewardTime * 0x3c;
        G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_2, 0, (TIMER_MESSAGE)0x8b,
                                    rewardTime, 0, 0);
        register int serverNo = G_CEnvironment()->m_serverEnvir.m_channelNo;
        cMyTrace tr(__PRETTY_FUNCTION__, 0x34, 0);
        tr("Power War Victorious Event Start! server(%d)", serverNo);
    }
}

// ---------------------------------------------------------------------------
// ORIG 0x08268b4a：IsEventing(0) 为真时清事件标志、清零 m_param（movw×2）并打日志。
#include "CPowerWarVictoriousEvent/EndEvent.inc"
{
    // [N7 封顶] IsEventing 虚调用 callee 寄存器 %edx(ORIG)↔%eax(4.4) 互换。
    if (IsEventing(0))
    {
        SetEventFlag(false);
        m_param.m_duration = 0;
        m_param.m_field2 = 0;
        cMyTrace tr(__PRETTY_FUNCTION__, 0x41, 0);
        tr("Power War Victorious Event End");
    }
}
