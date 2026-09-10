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
