MySQL* DB_Login::_GetDNFAccountDB()
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_1, E_SERVER_GROUP_NONE);
    return db;
}
