// ============================================================================
// _getGameDB（ORIG T 0x0840f4f8）/ _GetDNFAccountDB（ORIG T 0x08411bae）
// ============================================================================

MySQL* DB_Login::_getGameDB(ENUM_DB_HANDLE_IDX idx)
{
    return GlobalData::s_db_mgr->GetDBHandle(idx, E_SERVER_GROUP_NONE);
}
