int DBConnection::exec_query()
{
    clear_result_set();
    int db_ret = mysql_real_query(h_db, m_query, m_query_len);
    if (db_ret != 0)
    {
        m_db_err = (int)mysql_errno(h_db);
        // ORIG: m_db_err 内联（跨 G_TraceLog 调用保存在 ebx）。
        G_TraceLog()->sysLog(7, "Fail: mysql_real_query(), %d", m_db_err);
        if ((m_db_err == 0x7d5) || (m_db_err == 0x7dd) || (m_db_err == 0x7d3) ||
            (m_db_err == 0x7d6))
        {
            G_TraceLog()->sysLog(7, "Fail: DB connection lost, reconnecting...");
            int ping_ret = mysql_ping(h_db);
            if (ping_ret != 0)
            {
                // ORIG: 条件内联（sete+test），无 bool 局部。
                if (mysql_errno(h_db) == CR_SERVER_GONE_ERROR)
                {
                    if (mysql_real_connect(h_db, dbIp_, dbAcc_, dbPwd_, dbName_, dbPort_, 0, 0) ==
                        0)
                    {
                        G_TraceLog()->sysLog(7, "DB reconnection fail. err_no(%d)",
                                             mysql_errno(h_db));
                    }
                    else
                    {
                        G_TraceLog()->sysLog(7, "DB Reconnect By Server Gone Error");
                    }
                    G_TraceLog()->sysLog(
                        7,
                        "DB reconnection fail. %d, %d (dbname : %s, ip : %s, port : %d, id : %s, pwd : %s)",
                        ping_ret, mysql_errno(h_db), dbName_, dbIp_, dbPort_, dbAcc_, dbPwd_);
                }
                else
                {
                    G_TraceLog()->sysLog(7, "DB reconnection fail. %d, %d", ping_ret,
                                         mysql_errno(h_db));
                }
            }
            return 2;
        }
        if (m_db_err != 0x426)
        {
            G_TraceLog()->sysLog(7, "DB error occured (%d) Query(\'%s\')", mysql_errno(h_db),
                                 m_query);
        }
        return 1;
    }
    return 0;
