MySQL* DB_MouseRegister::_getDBHandleForDevideAccountDB()
{
    // ORIG：GetDBHandle(s_db_mgr, 6, 0)，结果经 -0xc 局部中转（O0 命名局部形态）。
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(
        DB_HANDLE_6, static_cast<ENUM_SERVER_GROUP>(0));
    return db;
}