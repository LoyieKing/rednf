void DBDispatcher::dispatch(Message* pMsg)
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
                // ORIG（df_game_r）不发射 INTERNALMSG_SERVICE_UNAVAILABLE 的 ctor
                // （无 _ZN31INTERNALMSG_SERVICE_UNAVAILABLEC[12]Ev，也无
                //  nsl::INTERNALMSG_HEADER::setCategory/setInternalMsgID/setSize）
                // ⇒ 该消息体在 ORIG 里是「零指令 POD 视图 + 显式字段写入」。
                // 头内 ctor 服务于其它二进制（Auction），不能删；本 TU 改以字节
                // 缓冲承载，逐条复现 ctor 体的写入序，不 odr-use 任何 inline 成员。
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
                pApp->super_Threads.getWorkThread(pNewMsg->getCellFromMessage()->GetInternalMsg()->workIndex)->PushTransaction(pNewMsg);
            }
        }
    }
    else
    {
        G_TraceLog()->sysLog(7,
            "DB : '%d' \xbf\xa1 \xb4\xeb\xc7\xd1 \xc7\xda\xb5\xe9\xb7\xaf\xb8\xa6 \xc3\xa3\xc1\xf6 \xb8\xf8\xc7\xcf\xbf\xb4\xbd\xc0\xb4\xcf\xb4\xd9.", dbtrid);
    }
