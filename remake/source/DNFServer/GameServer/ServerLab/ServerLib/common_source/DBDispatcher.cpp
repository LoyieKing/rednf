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

DBDispatcher::DBDispatcher()
{
}

void DBDispatcher::dispatch(Message* pMsg)
{
    CMsgCell* pCell = pMsg->getCellFromMessage();
    LPDBTR_HEADER pDbTr = pCell->GetDBTr();
    int dbtrid = pDbTr->dbtrId;
    int dbid = pDbTr->dbId;
    int ret = 0;
    IDBHandler* pHandler = pApp->super_IHandlers.getDBHandler(dbid);
    IDBHandler::DBHandlerFunc handle = pHandler->SearchDBHandlerFunc(dbtrid);
    if (handle == NULL)
    {
        G_TraceLog()->sysLog(7,
            "DB : '%d' \xbf\xa1 \xb4\xeb\xc7\xd1 \xc7\xda\xb5\xe9\xb7\xaf\xb8\xa6 \xc3\xa3\xc1\xf6 \xb8\xf8\xc7\xcf\xbf\xb4\xbd\xc0\xb4\xcf\xb4\xd9.", dbtrid);
        return;
    }
    pHandler = pApp->super_IHandlers.getDBHandler(dbid);
    ret = (pHandler->*handle)(pCell);
    if (ret != 0)
    {
        G_TraceLog()->sysLog(7,
            "DB : '%d' \xc3\xb3\xb8\xae\xb5\xa1 \xbd\xc7\xc6\xd0 \xc7\xcf\xb4\xb5\xbd\xc0\xb4\xcf\xb4\xd9.", dbtrid);
        if (ret == 2)
        {
            G_TraceLog()->sysLog(7,
                "DB : \xbf\xac\xb0\xe1\xc0\xcc \xb2\xf7\xbe\xee\xc1\xb3\xbd\xc0\xb4\xcf\xb4\xd9.");
            INTERNALMSG_SERVICE_UNAVAILABLE internalMsgServiceUnavailable;
            internalMsgServiceUnavailable.reason = 2;
            Message* pNewMsg = pApp->super_DataPools.getCommonDataPool(tlsThreadId)->createMessage(1);
            CMsgCell* pNewCell = pNewMsg->getCellFromMessage();
            *pNewCell << &internalMsgServiceUnavailable;
            WorkThread* pWorkThread = pApp->super_Threads.getWorkThread(pNewMsg->getCellFromMessage()->GetInternalMsg()->workIndex);
            pWorkThread->PushTransaction(pNewMsg);
        }
    }
}

} // namespace nsl
