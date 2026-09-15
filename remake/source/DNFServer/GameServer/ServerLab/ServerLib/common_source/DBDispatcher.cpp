// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 DBDispatcher/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// nsl::DBDispatcher.cpp
#include "DBDispatcher.h"
#include "IHandlers.h"
#include "Message.h"
#include "TraceLog.h"
#include "ServiceFactory.h"
#include "Threads.h"
#include "WorkThread.h"
#include "CommonDataPool.h"
#include "InternalMsgDesign.h"
#include "DBTransactionDesign.h"
#include "MsgCell.h"

namespace nsl {

#include "DBDispatcher/DBDispatcher.inc"
{
}

#include "DBDispatcher/dispatch.inc"
{
    CMsgCell* pCell = pMsg->getCellFromMessage();
    int dbtrid;
    int dbid;
    LPDBTR_HEADER pDbTr = pCell->GetDBTr();
    dbtrid = pDbTr->dbtrId;
    dbid = pDbTr->dbId;
    int ret = 0;
    IDBHandler::DBHandlerFunc handle = pApp->super_IHandlers.getDBHandler(dbid)->SearchDBHandlerFunc(dbtrid);
    if (handle != NULL)
    {
        // ORIG: member-pointer call without implicit base-subobject temp —
        // explicit (IHandler*) cast folds to direct lea (%edx,%ecx,1) forms.
        ret = (((IHandler*)pApp->super_IHandlers.getDBHandler(dbid))->*handle)(pCell);
        if (ret != 0)
        {
            G_TraceLog()->sysLog(7,
                "DB : '%d' \xc3\xb3\xb8\xae\xbf\xa1 \xbd\xc7\xc6\xd0 \xc7\xcf\xbf\xb4\xbd\xc0\xb4\xcf\xb4\xd9.", dbtrid);
            if (ret == 2)
            {
                G_TraceLog()->sysLog(7,
                    "DB : \xbf\xac\xb0\xe1\xc0\xcc \xb2\xf7\xbe\xee\xc1\xb3\xbd\xc0\xb4\xcf\xb4\xd9.");
#ifdef DNF_SERVERLIB_AUCTION_SHAPE
                // df_auction_r/df_point_r ORIG 实证：0x80b95dd-0x80b95e3 先 lea 局部再
                // call _ZN31INTERNALMSG_SERVICE_UNAVAILABLEC1Ev ⇒ 原码是普通具名对象 +
                // 单条 reason 赋值（auction/point 的 CMake 定义本宏）。
                INTERNALMSG_SERVICE_UNAVAILABLE svc;
                svc.reason = 2;
                Message* pNewMsg = pApp->super_DataPools.getCommonDataPool(tlsThreadId)->createMessage(1);
                CMsgCell* pNewCell = pNewMsg->getCellFromMessage();
                *pNewCell << &svc;
#else
                // df_game_r ORIG：不发射 INTERNALMSG_SERVICE_UNAVAILABLE 的 ctor
                // （无 _ZN31INTERNALMSG_SERVICE_UNAVAILABLEC[12]Ev，也无
                //  nsl::INTERNALMSG_HEADER::setCategory/setInternalMsgID/setSize）
                // ⇒ 该消息体在 game 的 ORIG 里是「零指令 POD 视图 + 显式字段写入」。
                unsigned char svcRaw[sizeof(INTERNALMSG_SERVICE_UNAVAILABLE)];
                INTERNALMSG_SERVICE_UNAVAILABLE* pSvc =
                    reinterpret_cast<INTERNALMSG_SERVICE_UNAVAILABLE*>(svcRaw);
                memset(pSvc, 0, sizeof(svcRaw));   // ctor: memset(this, 0, sizeof)
                pSvc->classification = 0;          // ctor: setCategory(0)
                pSvc->msg_no = 0;                  // ctor: setInternalMsgID(0)
                pSvc->sLength = sizeof(svcRaw);    // ctor: setSize(sizeof(...))
                pSvc->bWillDelete = false;
                pSvc->workIndex = 0;
                pSvc->mOwnerWorkId = 0;
                pSvc->bActiveJob = false;
                pSvc->reason = 2;
                Message* pNewMsg = pApp->super_DataPools.getCommonDataPool(tlsThreadId)->createMessage(1);
                CMsgCell* pNewCell = pNewMsg->getCellFromMessage();
                *pNewCell << pSvc;
#endif
                pApp->super_Threads.getWorkThread(pNewMsg->getCellFromMessage()->GetInternalMsg()->workIndex)->PushTransaction(pNewMsg);
            }
        }
    }
    else
    {
        G_TraceLog()->sysLog(7,
            "DB : '%d' \xbf\xa1 \xb4\xeb\xc7\xd1 \xc7\xda\xb5\xe9\xb7\xaf\xb8\xa6 \xc3\xa3\xc1\xf6 \xb8\xf8\xc7\xcf\xbf\xb4\xbd\xc0\xb4\xcf\xb4\xd9.", dbtrid);
    }
}

} // namespace nsl
