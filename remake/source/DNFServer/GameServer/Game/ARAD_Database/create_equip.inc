int ARAD::Arad_DB_Charac_Dispatcher::create_equip(SIG_CREATE_CHARAC* sig)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_3,
                                                  static_cast<ENUM_SERVER_GROUP>(0));
    if (db == 0)
    {
        LogManager::logFormat(
            1, "localjapan/Arad_Character.cpp",
            __PRETTY_FUNCTION__,
            0x95, "%s", "Database null!!");
        return -1;
    }
    // ORIG 荒诞哨兵比较照抄：(char*)sig + 0x4e01 == 0（即 sig==0xffffb1ff 时跳过写库）
    if ((char*)sig + 0x4e01 == 0)
    {
        return 0;
    }
    if (DATABASE::CREATE_QUERY::arad_sp_create_equip(db, sig) != 0)
    {
        return -1;
    }
    return 0;
}