int ARAD::Arad_DB_Charac_Dispatcher::create_charac_quest(SIG_CREATE_CHARAC* sig)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2,
                                                  static_cast<ENUM_SERVER_GROUP>(0));
    if (db == 0)
    {
        LogManager::logFormat(
            1, "localjapan/Arad_Character.cpp",
            __PRETTY_FUNCTION__,
            0xdb, "%s", "Database null!!");
        return -1;
    }
    if (DATABASE::CREATE_QUERY::arad_sp_create_charac_quest(db, sig) != 0)
    {
        return -1;
    }
    return 0;
