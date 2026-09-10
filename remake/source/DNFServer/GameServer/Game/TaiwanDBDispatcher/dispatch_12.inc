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
            // [P2S5] ORIG 0x0815aa1e/0x0815aa6e/0x0815aab6 三处实参为**后置**自增：
            // `mov -0x10(%ebp),%eax; addl $0x1,-0x10(%ebp); …; mov %eax,0x4(%esp)`
            // （加载旧值→内存自增→压旧值；4.4 实参左→右求值）。列索引 0-based
            // （MySQL::is_valid_col 允许 col>=0），故 ORIG 逐列传 0/1/2 ⇒ 用 idx++。
            if (db->get_int(idx++, v1) != 1)
                throw "category";
            if (db->get_int(idx++, v2) != 1)
                throw "restrict";
            if (db->get_char(idx++, c) != 1)
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
}
