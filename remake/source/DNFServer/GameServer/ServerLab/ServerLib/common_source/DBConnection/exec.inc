int DBConnection::exec(bool bLog)
{
    int iret;
    int i;
    for (i = 0; i < 5; i = i + 1)
    {
        iret = exec_query();
        if (iret == 1)
        {
            return 1;
        }
        if (iret == 0)
        {
            break;
        }
    }
    if (iret == 0)
    {
        m_res = (MYSQL_RES*)mysql_store_result(h_db);
        if (m_res != (MYSQL_RES*)0)
        {
            m_num_rows = (ulong)mysql_num_rows(m_res);
            m_num_fields = mysql_num_fields(m_res);
        }
        else
        {
            m_num_rows = 0;
            m_num_fields = 0;
        }
        return 0;
    }
    else
    {
        if (bLog)
        {
            G_TraceLog()->sysLog(7, "Database query error. The last query(\'%s\') has been lost.(%d)",
                                 m_query, m_db_err);
        }
        return 2;
    }
