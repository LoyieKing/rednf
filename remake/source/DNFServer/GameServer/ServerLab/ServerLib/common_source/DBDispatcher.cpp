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
    IDBHandler::DBHandlerFunc handle = pHandler->searchDBHandlerFunc(dbtrid);
    if (handle == NULL)
    {
        G_TraceLog()->sysLog(7, "DBDispatcher : can't find handler for dbtr : %d", dbtrid);
        return;
    }
    ret = (pHandler->*handle)(pCell);
    if (ret != 0)
    {
        G_TraceLog()->sysLog(7, "DBDispatcher : dbtr : %d", dbtrid);
        if (ret == 2)
        {
            G_TraceLog()->sysLog(7, "DBDispatcher : dbtr : %d", dbtrid);
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
