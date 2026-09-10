MySQL* EventFCCombo::GetDBHandle()
{
    return GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)1,
                                             (ENUM_SERVER_GROUP)0);
