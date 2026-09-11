MySQL* DB_Login::_getGameDB(ENUM_DB_HANDLE_IDX idx)
{
    return GlobalData::s_db_mgr->GetDBHandle(idx, E_SERVER_GROUP_NONE);
}