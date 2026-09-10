bool DBInsertGiveBoxEventReward::dispatch(int code, int uid, Stream* stream)
{
    try
    {
        DBInsertGiveBoxEventReward::MSG* sig = stream->GetOutBuffer<DBInsertGiveBoxEventReward::MSG>();
        if (sig == 0)
            throw "context null";
        MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)9,
                                                      (ENUM_SERVER_GROUP)0);
        if (db == 0)
            throw "handle null";
        time_t t = time(0);
        struct tm tm;
        localtime_r(&t, &tm);
        struct tm tm2 = tm;
        tm2.tm_hour = G_CEnvironment()->m_serverEnvir.m_fatigueTime;
        tm2.tm_min = 0;
        tm2.tm_sec = 0;
        time_t now = mktime(&tm2);
        if (tm.tm_hour < tm2.tm_hour)
            now -= 0x15180;
        db->set_query(
                "seLect m_id from event_1306_account_reward_2nd where m_id=%s and occ_date > FROM_UNIXTIME(%d)",
                NumberToString(sig->m_accId, 0), (int)now);
        if (!db->exec(1))
            throw "select_exec";
        int nRows = db->get_n_rows();
        if (nRows == 0)
        {
            // b28c-L20: ORIG 0x0815d5c6 对 m_characNo(int) 直发
            // `mov 0x4(%eax),%ebx`（跨 NumberToString 调用保活）；写
            // `(unsigned int)sig->m_characNo` 的异型化实参会多一条
            // `mov %eax,%ebx` 中转 ⇒ 保持字段同型（int）直传（P211 反向）。
            db->set_query(
                    "inSert into event_1306_account_reward_2nd (m_id , charac_no , occ_date ) values (%s , %u , now())",
                    NumberToString(sig->m_accId, 0), sig->m_characNo);
            if (!db->exec(1))
                throw "insert_exec";
            CStreamGuard guard(
                GlobalData::s_stream_pool->Acquire(
                    "localtaiwan/Dispatcher/Taiwan_DbThread.cpp", 0x3a5),
                true);
            Taiwan::internal_stream(guard, (TaiwanInternalPack::T)0x10, uid);
            DBInsertGiveBoxEventReward::MSG* outSig =
                guard.operator->()->GetInBuffer<DBInsertGiveBoxEventReward::MSG>();
            if (outSig == 0)
                throw "stream";
            outSig->m_characNo = sig->m_characNo;
            outSig->m_accId = sig->m_accId;
            GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
        }
    }
    catch (const char* e)
    {
        LogManager::logFormat(
            1, "localtaiwan/Dispatcher/Taiwan_DbThread.cpp", __PRETTY_FUNCTION__,
            0x3b4,
            "[Taiwan, GiveMeBox] GIVE_ME_BOX_EVENT. (error:%s)", e);
        return 0;
    }
    // b28c-L20: ORIG 0x0815d3ee 成功返回值在函数尾共享块物化（mov $1,%ebx 位于
    // CStreamGuard 析构之后、与 catch 的 mov $0,%ebx 汇合）⇒ return 1 落在
    // try 之外，不能写在 try 块内（4.4 会在 put 后立刻物化，ndiff 5）。
    return 1;
}
