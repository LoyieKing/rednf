bool CQueryCounter::SaveQuery(const char* query, int& qid)
{
    MySQL* mysql = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_6, E_SERVER_GROUP_NONE);
    char escaped[0xc001];
    memset(escaped, 0, 0xc001);
    mysql->escape_string(escaped, query);
    mysql->set_query("inSert into log_query_ref(query, query_hash) values('%s',password('%s'))",
                     escaped, escaped);
    if (mysql->exec(true))
    {
        qid = GetIdentityFromDB(mysql);
        return true;
    }
    mysql->set_query("seLect q_id from log_query_ref where query_hash=password('%s')",
                     escaped, escaped);
    if (mysql->exec(true))
    {
        if (mysql->get_n_rows() == 0)
        {
            return false;
        }
        if (mysql->fetch())
        {
            if (mysql->get_int(0, qid))
            {
                return true;
            }
            cMyTrace trace(__PRETTY_FUNCTION__, 0x277, 5);
            trace("SaveQuery, get_int(0) ERROR");
            return false;
        }
        cMyTrace trace(__PRETTY_FUNCTION__, 0x271, 5);
        trace("SaveQuery, fetch() ERROR");
        return false;
    }
    cMyTrace trace(__PRETTY_FUNCTION__, 0x265, 5);
    trace("SaveQuery, exec() ERROR");
    return false;
