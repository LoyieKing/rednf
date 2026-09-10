void NSLDBThread::loop(void* temp)
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
