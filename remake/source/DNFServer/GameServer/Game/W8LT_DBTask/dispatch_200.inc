bool DB_UpdatePvPLastPlayTime::dispatch(int /*userIdx*/, int /*slot*/,
                                        Stream* stream)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_10,
                                                  E_SERVER_GROUP_NONE);
    Packet_PvPResult* sig = stream->GetOutBuffer<Packet_PvPResult>();
    DB_PvPResultSchoolView* v =
        reinterpret_cast<DB_PvPResultSchoolView*>(sig);
    for (int i = 0; (unsigned char)v->m_count > i; ++i)
    {
        for (int j = 0; j <= 1; ++j)
        {
            db->set_query(
                "upDate school_member set last_play_time=NOW()  where m_id=%s and school_id=%d",
                NumberToString(v->m_mId[j][i], 0), v->m_schoolId[j][i]);
            if (db->exec(true) != 1)
                return 0;
        }
    }
    return 1;
