// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 TimeManager/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
#include "TimeManager.h"
#include "TimerThread.h"
#include "InternalMsg.h"
#include "TraceLog.h"
#include "ServiceFactory.h"
#include "Threads.h"
#include "WorkThread.h"
#include "Message.h"

namespace nsl {

#include "TimeManager/TimeManager.inc"
{
    last_tick = 0;
    cur_tick = 0;
    elapsedTick = 0;
    mtotal_entity = 0;
}

#include "TimeManager/dtor_TimeManager.inc"
{
}

#include "TimeManager/insert2PeriodQueue.inc"
{
    if (!timePeriodMap.Insert(ent->entNo, ent))
    {
        G_TraceLog()->sysLog(8, "Fail: ==>Can't Insert TimeEntity!! uid=%d, pid=%d", ent->entNo, ent->proc_id);
        G_TraceLog()->sysLog(7, "Fail: ==>Can't Insert TimeEntity!! uid=%d, pid=%d", ent->entNo, ent->proc_id);
    }
    else
    {
        G_TraceLog()->sysLog(8, "Success: ==>Insert TimeEntity!! uid=%d, pid=%d", ent->entNo, ent->proc_id);
    }
}

#include "TimeManager/delete2PeriodQueue.inc"
{
    if (!timePeriodMap.Remove(ent->entNo))
    {
        G_TraceLog()->sysLog(7, "Fail: ==>Not found Time Entity entNo(%d), id(%d)", ent->entNo, ent->proc_id);
    }
    ent->setTerminated();
    InternalMsg* pArg = ent->getArg();
    pApp->super_Threads.getWorkThread(pArg->workIndex)->PushTransaction((IMessageStruct*)ent);
}

#include "TimeManager/onTime.inc"
{
    cur_tick = pApp->getTick();
    elapsedTick = cur_tick - last_tick;
    iter = timePeriodMap.InorderBegin();
    while (iter.GetDataPtr() != NULL)
    {
        ITimeEntity* ent = *iter.GetDataPtr();
        ent->accumulated_tick += elapsedTick;
        if ((unsigned long long)ent->check_period <= (unsigned long long)ent->accumulated_tick)
        {
            InternalMsg* pArg = ent->getArg();
            pApp->super_Threads.getWorkThread(pArg->workIndex)->PushTransaction((IMessageStruct*)ent);
            ent->accumulated_tick = 0;
        }
        ++iter;
    }
    last_tick = cur_tick;
}

} // namespace nsl
