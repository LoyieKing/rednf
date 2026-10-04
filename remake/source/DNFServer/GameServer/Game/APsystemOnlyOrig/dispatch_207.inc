// ============================================================================
// DB_ReqSaveCrazyLevelupEvent::dispatch —— ORIG 0x0842527a（报告 dispatch.md
// 全文）
// ============================================================================
// GetOutBuffer<SIG_SAVE_CRAZY_LEVELUP_EVENT> → GetDBHandle(4,0)；先 inSert
// log_fighter_event(m_id=num(+0x05), event_id=+0x00, charac_no=+0x09)；exec
// 失败 → upDate occ_time=now(),occ_cnt=occ_cnt+1（同键）；再失败返 0。
bool DB_ReqSaveCrazyLevelupEvent::dispatch(int /*userIdx*/, int /*slot*/,
                                           Stream* stream)
{
    SIG_SAVE_CRAZY_LEVELUP_EVENT* sig =
        /*SIG_SAVE_CRAZY_LEVELUP_EVENT*/(
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
}