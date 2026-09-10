// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 TimerThread/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
#include <stdio.h>
#include <time.h>

#include "TimerThread.h"
#include "TraceLog.h"
#include "System.h"

namespace nsl {

#include "TimerThread/TimerThread.inc"
    : timeReqQueue()
{
    pthread_mutex_init(&timerLock, NULL);
}

#include "TimerThread/dtor_TimerThread.inc"
{
}

#include "TimerThread/PushTimeReqEvent.inc"
{
    pthread_mutex_lock(&timerLock);
    timeReqQueue.push(pTEvent);
    G_TraceLog()->sysLog(8, "TEVENT PUSH ID=%d", pTEvent->proc_id);
    pthread_mutex_unlock(&timerLock);
}

#include "TimerThread/PopTimeReqEvent.inc"
{
    pthread_mutex_lock(&timerLock);
    if (timeReqQueue.size() == 0)
    {
        pthread_mutex_unlock(&timerLock);
        return NULL;
    }
    ITimeEntity* ent = timeReqQueue.front();
    // ORIG: if (bWillDelete) insert; else log+delete  (test/je delete path)
    if (ent->bWillDelete)
    {
        insert2PeriodQueue(ent);
    }
    else
    {
        G_TraceLog()->sysLog(8, "11Time Event Delete!!!!\n");
        delete2PeriodQueue(ent);
    }
    timeReqQueue.pop();
    pthread_mutex_unlock(&timerLock);
    return ent;
}

#include "TimerThread/loop.inc"
{
    // ORIG: sign-extend time() to 64-bit via sar, sub/sbb into locals, cmpl mem for unsigned >
    do
    {
        long long startTime = (long long)time(NULL);
        PopTimeReqEvent();
        onTime();
        long long endTime = (long long)time(NULL);
        // ORIG：无显式 unsigned 转型（-O0 先装 startTime 再装 endTime 的
        // 64 位减法装载顺序；显式转型会先装 endTime，顺序相反）。
        unsigned long long elapsedTime = endTime - startTime;
        if (elapsedTime > (unsigned long long)0x14)
        {
            puts("TimerThread spent more time than NEXT_CHECK_TIME");
            printf("Elapsed Time: %lld\n", (long long)elapsedTime);
            printf("Map size: %d\n", timePeriodMap.Size());
            // ORIG: explicit continue reproduces nop; jmp back-edge merge
            continue;
        }
        else
        {
            if (0x14 - (int)elapsedTime < 0)
            {
                TSystem<LinuxSystem>::sleep(0);
                printf("\xBD\xBD\xB8\xB3 \xC5\xB8\xC0\xD3 : %d\n", 0x14 - (int)elapsedTime);
            }
            else
            {
                TSystem<LinuxSystem>::sleep(0x14 - (int)elapsedTime);
            }
        }
    } while (true);
}

} // namespace nsl
