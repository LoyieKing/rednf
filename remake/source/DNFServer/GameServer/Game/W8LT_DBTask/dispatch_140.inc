bool DB_SaveCleanpadPoint::dispatch(int userIdx, int slot, Stream* stream)
{
    unsigned int point;
    unsigned int mId;
    *stream >> point;
    *stream >> mId;
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_6,
                                                  E_SERVER_GROUP_NONE);
    db->set_query("upDate member_login set cleanpad_point=%d where m_id=%s",
                  point, NumberToString(mId, 0));
    if (!db->exec(true))
        return 0;
    return 1;
