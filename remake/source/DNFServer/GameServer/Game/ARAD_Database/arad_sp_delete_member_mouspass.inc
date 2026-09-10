int LOGIN_QUERY::arad_sp_delete_member_mouspass(SIG_LOGIN_DATA* data)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_6,
                                                  static_cast<ENUM_SERVER_GROUP>(0));
    if (db == 0)
    {
        LogManager::logFormat(
            1, "localjapan/Arad_DB_Query.cpp",
            __PRETTY_FUNCTION__,
            0x345, "%s", "Database null!!");
        return -1;
    }
    if (db->select("deLete from member_mousepass where m_id=%d",
                   data->m_accId) != 0)
    {
        return -1;
    }
    // ORIG 0x0818c6ab：64 位行数 or %edx,%eax 判零后走 sete 物化
    // （sete %al; test %al,%al; je），源用 if(x==0) return -1 而非三目（三目 O0 直出 jne）。
    if (db->getAffectedRowCount() == 0)
        return -1;
    return 0;
