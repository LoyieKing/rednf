int MySQL::exec_query()
{
    clear_result_set();
    int ret = mysql_real_query(m_dbHandle, m_query, m_queryLen);
    if (ret != 0)
    {
        m_dbError = mysql_errno(m_dbHandle);
        if (m_dbError == 0x7d5 || m_dbError == 0x7dd || m_dbError == 0x7d6)
        {
            // b28d-lane11：P221 全表达式临时链（翻互斥块对象槽方向）。
            cMyTraceNoop()("DB connection lost, reconnecting...");
            int ping = mysql_ping(m_dbHandle);
            if (ping != 0)
            {
                if (mysql_errno(m_dbHandle) == 0x7d6)
                {
                    // b28d-lane11：P0 内联（ORIG 0x083f54c4 `call mysql_real_connect;
                    // test %eax,%eax; sete %al; test %al,%al; je` 无 conn 槽往返）。
                    if (mysql_real_connect(m_dbHandle, m_host, m_user, m_passwd,
                                           m_db, m_port, 0, 0) == 0)
                    {
                        LogManager::logFormat(1, "DBMgr.cpp", "bool MySQL::exec_query()", 0x19a,
                                              "DB reconnection fail. err_no(%d)",
                                              mysql_errno(m_dbHandle));
                    }
                    else
                    {
                        LogManager::logFormat(1, "DBMgr.cpp", "bool MySQL::exec_query()", 0x19e,
                                              "DB Reconnect By Server Gone Error");
                    }
                    LogManager::logFormat(1, "DBMgr.cpp", "bool MySQL::exec_query()", 0x1a1,
                                          "DB reconnection fail. %d (dbname : %s, ip : %s, port : %d, id : %s)",
                                          ping, m_db, m_host, m_port, m_user);
                }
            }
            return 2;
        }
        if (m_dbError != 0x426)
        {
            // b28d-lane11 真语义修正：ORIG 0x083f5573-0x083f558b 此处为 fresh
            // `call mysql_errno(m_dbHandle)`（`lea 0x1c(%eax),%ebx` 先缓存 m_query，
            // call 后 %eax 压 0x14(%esp)），非读缓存字段 m_dbError（字段读仅用于
            // 紧随其后的 ==0x7d6 判定，ORIG 0x083f558f）。
            LogManager::logFormat(1, "DBMgr.cpp", "bool MySQL::exec_query()", 0x1af,
                                  "DB error occured (%d) Query('%s')",
                                  mysql_errno(m_dbHandle), m_query);
            if (m_dbError == 0x7d6)
            {
                cMyTrace("bool MySQL::exec_query()", 0x1b2, 5)(
                    "DB error occured Check Connection First, Must Be Not Connected!");
            }
        }
        return 1;
    }
    return 0;
