bool DB_UpdateSchoolPoint::dispatch(int /*userIdx*/, int /*slot*/,
                                    Stream* stream)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_6,
                                                  E_SERVER_GROUP_NONE);
    SIG_UPDATE_SCHOOL_POINT* sig =
        stream->GetOutBuffer<SIG_UPDATE_SCHOOL_POINT>();
    db->set_query(
        "upDate event_school_point set point=%d+point where m_id=%s",
        sig->m_point, NumberToString(sig->m_mId, 0));
    if (!db->exec(true))
        return 0;
    return 1;
