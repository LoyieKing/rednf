// ============================================================================
// DB_LoadCharac::_getGameDB @0x08408c0a
// 直读 GlobalData::s_db_mgr → GetDBHandle(2, 0)，返回 MySQL*（本函数体不
// 用 this，但 ORIG 调用点 _getCharacInfo 仍按成员压 this，故非 static）。
// ============================================================================
MySQL* DB_LoadCharac::_getGameDB()
{
    return GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)2,
                                             (ENUM_SERVER_GROUP)0);
}