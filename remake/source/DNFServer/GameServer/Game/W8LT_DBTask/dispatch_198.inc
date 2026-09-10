bool DB_TOD_Update_UserState::dispatch(int /*userIdx*/, int /*slot*/,
                                       Stream* stream)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2,
                                                  E_SERVER_GROUP_NONE);
    SIG_REQUEST_TOD_USER_STATE* sig =
        stream->GetOutBuffer<SIG_REQUEST_TOD_USER_STATE>();
    if (sig->m_firstEnterTime == 0)
        return 1;
    db->set_query(
        "upDate charac_tower_despair set first_layer_start_date=from_unixtime(%d), today_enter_count=%d, last_clear_layer=%d, enter_count_by_week=%d, m_date=from_unixtime(%d), last_clear_date=from_unixtime(%d) where charac_no=%u",
        sig->m_firstEnterTime, sig->m_enterCount,
        (unsigned int)sig->m_todayLayer.GetLayer(),
        (int)(short)sig->m_lastEnterLayer, sig->m_lastEnterTime,
        sig->m_lastClearTime, sig->m_characNo);
    if (!db->exec(true))
        return 0;
    if (db->getAffectedRowCount() == 0)
    {
        db->set_query(
            "inSert into charac_tower_despair(charac_no,first_layer_start_date,today_enter_count,last_clear_layer,enter_count_by_week,m_date,last_clear_date) values(%u,from_unixtime(%d),%d,%d,%d,from_unixtime(%d),from_unixtime(%d))",
            sig->m_characNo, sig->m_firstEnterTime, sig->m_enterCount,
            (unsigned int)sig->m_todayLayer.GetLayer(),
            (int)(short)sig->m_lastEnterLayer, sig->m_lastEnterTime,
            sig->m_lastClearTime);
        if (!db->exec(true))
            return 0;
    }
    return 1;
