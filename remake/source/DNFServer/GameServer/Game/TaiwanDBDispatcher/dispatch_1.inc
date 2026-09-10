bool CheckStayTimeEvent::dispatch(int code, int uid, Stream* stream)
{
    try
    {
        // ORIG 0x0815b362：GetOutBuffer<Taiwan::SigStayTimeEvent>（MoreTypes.h 真类型；
        // 禁本地同构视图——实例化类型不符产生 EXTRA 调用，P167 判据）。
        Taiwan::SigStayTimeEvent* sig =
            stream->GetOutBuffer<Taiwan::SigStayTimeEvent>();
        if (sig == 0)
            throw "context null";
        MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)9,
                                                      (ENUM_SERVER_GROUP)0);
        if (db == 0)
            throw "handle null";
        // ORIG 0x0815b424：set_query 返回值不检查（失败留给 exec 的 throw）
        db->set_query(
            "seLect reward_flag from event_stay_time_charac where m_id=%s",
            NumberToString(sig->m_accId, 0));
        if (!db->exec(true))                 // ORIG：exec(true) 失败 throw
            throw "select_exec";
        if (db->get_n_rows() == 0)
        {
            // ORIG 0x0815b4ac：insert set_query 不检查，exec(true) 失败 throw
            db->set_query(
                "inSert into event_stay_time_charac values('%s', 1, 1, now())",
                NumberToString(sig->m_accId, 0));
            if (!db->exec(true))
                throw "insert_exec";
            sendStayTimeData(uid);
        }
        else
        {
            if (!db->fetch())
                throw "fetch";
            int flag = 0;
            if (!db->get_int(0, flag))
                throw "get_int";
            // ORIG 0x0815b5a1：`test; jne 0x2ce`（flag!=0 直跳 return 块），
            // Ghidra `if (flag==0) { update } return 1`。原 `if(flag!=0)return 1`
            // 被 4.4 反成 je+mov$1（+4 条）。改 flag==0 正入 update、return 1
            // 统一走末尾（P59）。
            if (flag == 0)
            {
                // ORIG：update set_query 不检查，exec(true) 失败 throw
                db->set_query(
                    "upDate event_stay_time_charac set reward_flag=1, reward_count=reward_count+1, mod_date=now() where m_id = %s",
                    NumberToString(sig->m_accId, 0));
                if (!db->exec(true))
                    throw "update_exec";
                sendStayTimeData(uid);
            }
        }
        return 1;
    }
    catch (const char* e)
    {
        LogManager::logFormat(
            1, "localtaiwan/Dispatcher/Taiwan_DbThread.cpp", __PRETTY_FUNCTION__,
            0x18a,
            "[Taiwan, Event] select event_stay_time_charac. (error:%s)", e);
        return 0;
    }
