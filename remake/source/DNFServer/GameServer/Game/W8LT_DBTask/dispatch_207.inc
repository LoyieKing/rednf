bool DB_ReqSaveCrazyLevelupEvent::dispatch(int /*userIdx*/, int /*slot*/,
                                           Stream* stream)
{
    DB_SaveCrazyLevelupEventView* sig =
        reinterpret_cast<DB_SaveCrazyLevelupEventView*>(
            stream->GetOutBuffer<SIG_SAVE_CRAZY_LEVELUP_EVENT>());
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_4,
                                                  E_SERVER_GROUP_NONE);
    db->set_query(
        "inSert into log_fighter_event(m_id,event_id,charac_no,occ_time,occ_cnt) values(%s,'%s',%d,now(),1)",
        NumberToString(sig->m_memberId, 0), sig->m_eventId, sig->m_characNo);
    if (db->exec(true) != 1)
    {
        db->set_query(
            "upDate log_fighter_event set occ_time=now(),occ_cnt=occ_cnt+1 where m_id=%s and event_id='%s' and charac_no=%d",
            NumberToString(sig->m_memberId, 0), sig->m_eventId,
            sig->m_characNo);
        if (db->exec(true) != 1)
            return 0;
    }
    return 1;
