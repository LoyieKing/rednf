MySQL* DB_LoadCharac::_getGameDB()
{
    return GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)2,
                                             (ENUM_SERVER_GROUP)0);
