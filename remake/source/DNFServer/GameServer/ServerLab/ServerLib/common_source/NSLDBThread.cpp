// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 NSLDBThread/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// nsl::NSLDBThread.cpp
#include <pthread.h>

#include "NSLDBThread.h"
#include "ServiceFactory.h"
#include "Dispatchers.h"
#include "DBDispatcher.h"
#include "DataPool.h"
#include "Message.h"
#include "CommonDataPool.h"
#include "InternalMsgDesign.h"
#include "DBTransactionDesign.h"
#include "MsgCell.h"

namespace nsl {

#include "NSLDBThread/NSLDBThread.inc"
    : dbQueue()
{
    mQueueSize = 0;
    mTransactionCntPerSec = 0;
    pthread_mutex_init(&dbLock, NULL);
    pthread_cond_init(&isFull, NULL);
    pthread_cond_init(&isEmpty, NULL);
}

#include "NSLDBThread/dtor_NSLDBThread.inc"
{
}

#include "NSLDBThread/PushTransaction.inc"
{
    pthread_mutex_lock(&dbLock);
    mQueueSize = mQueueSize + 1;
    dbQueue.push(pMessage);
    pthread_cond_signal(&isEmpty);
    pthread_mutex_unlock(&dbLock);
}

#include "NSLDBThread/PopTransaction.inc"
{
    pthread_mutex_lock(&dbLock);
    while (dbQueue.size() == 0)
    {
        pthread_cond_wait(&isEmpty, &dbLock);
    }
    Message* pMsg = dbQueue.front();
    dbQueue.pop();
    mQueueSize = mQueueSize - 1;
    pthread_mutex_unlock(&dbLock);
    return pMsg;
}

#include "NSLDBThread/loop.inc"
{
    Message* pMsg;
    DBDispatcher* pDbDispatcher = pApp->super_Dispatchers.getDBDispatcher();
    DBTR_HEADER* pDbTr;
    while (true)
    {
        pMsg = PopTransaction();
        pDbTr = pMsg->getCellFromMessage()->GetDBTr();
        if (!pDbTr->mbWillDelete)
        {
            pDbDispatcher->dispatch(pMsg);
            pDbTr->mbWillDelete = true;
            pApp->super_DataPools.getCommonDataPool(tlsThreadId)->destroyMessage(pMsg);
            mTransactionCntPerSec = mTransactionCntPerSec + 1;
        }
    }
}

} // namespace nsl
