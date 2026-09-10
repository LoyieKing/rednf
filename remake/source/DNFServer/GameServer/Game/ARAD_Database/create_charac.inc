int ARAD::Arad_DB_Charac_Dispatcher::create_charac(SIG_CREATE_CHARAC* sig)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2,
                                                  static_cast<ENUM_SERVER_GROUP>(0));
    if (db == 0)
    {
        LogManager::logFormat(
            1, "localjapan/Arad_Character.cpp",
            __PRETTY_FUNCTION__,
            0x4e, "%s", "Database null!!");
        return -1;
    }
    if (DATABASE::CREATE_QUERY::arad_sp_create_character(db, sig) != 0)
    {
        return -1;
    }
    if (DATABASE::CREATE_QUERY::arad_sp_get_characer_index(db, sig) != 0)
    {
        return 0x59;                     // ORIG 唯一非 -1 错误码
    }
    return 0;
