// ----------------------------------------------------------------------------
// EventFCCombo::GetDBHandle —— ORIG W 0x0844dc7c：
//   return DBMgr::GetDBHandle(GlobalData::s_db_mgr, 1, 0);
// ----------------------------------------------------------------------------
MySQL* EventFCCombo::GetDBHandle()
{
    return GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)1,
                                             (ENUM_SERVER_GROUP)0);
}