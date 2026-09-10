bool LoadServiceRestrictPolicy::dispatch(int code, int uid, Stream* stream)
{
    try
    {
        MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)1,
                                                      (ENUM_SERVER_GROUP)0);
        if (db == 0)
            throw "handle null";
        int serverGroup = G_CEnvironment()->get_server_group();
        if (!db->set_query(
                "seLect category, restrict_code, restrict_value from dnf_restrict_state where server_group=%d",
                serverGroup))
            throw "set_query";
        if (!db->exec(true))
            throw "exec";
        Taiwan::SigLoadRestrictPolicy sig;
        sig.init();
        while (db->fetch())
        {
            int idx = 0;
            int v1 = 0;
            int v2 = 0;
            char c = 0;
            ++idx;
            if (db->get_int(idx, v1) != 1)
                throw "category";
            ++idx;
            if (db->get_int(idx, v2) != 1)
                throw "restrict";
            ++idx;
            if (db->get_char(idx, c) != 1)
                throw "values";
            if (sig.m_restrictValue == 0 || sig.m_restrictValue == v1)
            {
                // [b28d-solo-TaiwanDBDispatcher] 真 bug 修正：ORIG 0x0815a8f2 两处
                // set 压栈序 (0x4=v1,-0x1c / 0x8=v2,-0x20 / 0xc=c) ⇒ 原码 set(v1,v2,c)；
                // 旧写 set(v2,v1,c) 使 m_restrictValue 与 m_extra 索引互换（可观察行为错）。
                sig.set(v1, v2, c);
            }
            else
            {
                sendRestrictData(sig);
                sig.init();
                sig.set(v1, v2, c);
            }
        }
        sendRestrictData(sig);
        return 1;
    }
    catch (const char* e)
    {
        LogManager::logFormat(
            1, "localtaiwan/Dispatcher/Taiwan_DbThread.cpp", __PRETTY_FUNCTION__,
            0xd2,
            "[Taiwan, Restrict] select dnf_restrict_state. (error:%s)", e);
        return 0;
    }
