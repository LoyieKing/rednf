// ORIG 0x0818c6c4：upDate member_mousepass.fail_cnt。
int LOGIN_QUERY::arad_sp_update_fail_record(SIG_LOGIN_DATA* data)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_6,
                                                  static_cast<ENUM_SERVER_GROUP>(0));
    if (db == 0)
    {
        LogManager::logFormat(
            1, "localjapan/Arad_DB_Query.cpp",
            __PRETTY_FUNCTION__,
            0x365, "%s", "Database null!!");
        return -1;
    }
    if (db->select("upDate member_mousepass set fail_cnt=%d where m_id=%d",
                   data->m_passPadFailCnt,
                   data->m_accId) != 0)
    {
        return -1;
    }
    return 0;
}