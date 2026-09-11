// ORIG 0x08299912：m_db = DBMgr::GetDBHandle((ENUM_DB_HANDLE_IDX)6, (ENUM_SERVER_GROUP)0)
// 后立即 LoadGmListFromDb()。
Gm_List::Gm_List()
{
    m_db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)6,
                                             (ENUM_SERVER_GROUP)0);
    LoadGmListFromDb();
}