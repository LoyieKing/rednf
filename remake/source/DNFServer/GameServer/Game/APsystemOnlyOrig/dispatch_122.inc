// ============================================================================
// DB_SavePuUserCheck::dispatch —— ORIG 0x0844bfb8（报告 dispatch.md）。
// GetDBHandle(DB_HANDLE_1,0) 空返 0；Stream>>m_id(u32) → seLect m_id from
// pu_user_list where m_id=%u → exec 失败返 0；get_n_rows()==0 时 inSert
// into pu_user_list(m_id) values(%u) → inSert exec 失败返 0；全绿返 1。
// ============================================================================
bool DB_SavePuUserCheck::dispatch(int userIdx, int slot, Stream* stream)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)1,
                                                  (ENUM_SERVER_GROUP)0);
    if (db == 0)
        return 0;
    unsigned int mId = 0;
    *stream >> mId;
    db->set_query("seLect m_id from pu_user_list where m_id = %u", mId);
    if (!db->exec(true))
        return 0;
    if (db->get_n_rows() == 0)
    {
        db->set_query("inSert into pu_user_list(m_id) values(%u)", mId);
        if (!db->exec(true))
            return 0;
    }
    return 1;
}