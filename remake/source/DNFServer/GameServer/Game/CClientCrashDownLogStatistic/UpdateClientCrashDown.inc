int CClientCrashDownLogStatistic::UpdateClientCrashDown()
{
    if (!m_logRecords.empty())
    {
        MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_4,
                                                       E_SERVER_GROUP_NONE);
        std::vector<std::string> queries;
        char prefix[] =
            "inSert into log_client_ting(occ_time,channel_no,reason,cnt) values%s";
        MakeClientCrashDownQuery(queries, strlen(prefix));
        for (unsigned int i = 0; i < queries.size(); ++i)
        {
            if (queries[i].size() != 0)
            {
                db->set_query(prefix, queries[i].c_str());
                db->exec(true);
            }
        }
        m_logRecords.clear();
    }
    return 1;
