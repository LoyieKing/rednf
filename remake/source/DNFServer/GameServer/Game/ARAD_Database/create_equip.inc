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
    // ORIG 荒诞哨兵：(char*)sig + 0x4e01 == 0（&sig->m_giftItem 空）
    if (&sig->m_giftItem == 0)
    {
        return 0;
    }
    if (DATABASE::CREATE_QUERY::arad_sp_create_equip(db, sig) != 0)
    {
        return -1;
    }
    return 0;
}