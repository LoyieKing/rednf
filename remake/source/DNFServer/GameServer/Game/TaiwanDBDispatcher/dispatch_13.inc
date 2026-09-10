bool SaveServiceRestrictPolicy::dispatch(int code, int uid, Stream* stream)
{
    try
    {
        Taiwan::SigLoadRestrictPolicy* sig =
            stream->GetOutBuffer<Taiwan::SigLoadRestrictPolicy>();
        if (sig == 0)
            throw "context null";
        MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)1,
                                                      (ENUM_SERVER_GROUP)0);
        if (db == 0)
            throw "handle null";
        int serverGroup = G_CEnvironment()->get_server_group();
        for (int i = 1; i <= sig->m_count; ++i)
        {
            if (!db->set_query(
                    "inSert ignore into dnf_restrict_state (server_group , category , restrict_code , restrict_value , mod_date , reg_date) values(%d, %d, %d, '%d', now(), now())",
                    serverGroup, sig->m_restrictValue, i,
                    static_cast<int>(static_cast<signed char>(sig->m_extra[i]))))
                throw "set_query";
            if (!db->exec(true))
                throw "insert_exec";
        }
        for (int i = 1; i <= sig->m_count; ++i)
        {
            if (!db->set_query(
                    "inSert ignore into dnf_restrict_info (category , restrict_code , restrict_str , reg_date) values(%d, %d, '%s', now())",
                    sig->m_restrictValue, i,
                    sig->m_data[i]))
                throw "set_query";
            if (!db->exec(true))
                throw "insert_exec";
        }
        return 1;
    }
    catch (const char* e)
    {
        LogManager::logFormat(
            1, "localtaiwan/Dispatcher/Taiwan_DbThread.cpp", __PRETTY_FUNCTION__,
            0x10e,
            "[Taiwan, Restrict] select dnf_restrict_state. (error:%s)", e);
        return 0;
    }
