// ORIG 0x0815ad3c T SaveServiceRestrictPolicy::dispatch（报告 dispatch.md）：
// GetOutBuffer<SigLoadRestrictPolicy> null→throw "context null"；GetDBHandle(1,0)
// null→throw "handle null"；两循环（i=1..sig->m_count）：dnf_restrict_state
// values(%d,%d,%d,'%d')= (sg, restrictValue, i, m_extra[i])；dnf_restrict_info
// values(%d,%d,'%s')= (sg, restrictValue, i, &m_data[i*0x2d]+8)；set_query 失败
// throw "set_query" / exec 失败 throw "insert_exec"。catch 0x10e 返 0。
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
}
