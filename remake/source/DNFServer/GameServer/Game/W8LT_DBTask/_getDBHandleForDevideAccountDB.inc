MySQL* DB_SaveUserHandicap::_getDBHandleForDevideAccountDB()
{
    ENUM_DB_HANDLE_IDX idx = (ENUM_DB_HANDLE_IDX)1;
    return GlobalData::s_db_mgr->GetDBHandle(idx, (ENUM_SERVER_GROUP)0);
