bool DB_SaveProperDungeonClearCount::dispatch(int userIdx, int slot,
                                              Stream* stream)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2,
                                                  E_SERVER_GROUP_NONE);
    SIG_SAVE_BLOOD_CLEAR_COUNT* sig =
        stream->GetOutBuffer<SIG_SAVE_BLOOD_CLEAR_COUNT>();
    db->set_query(
        "upDate event_dungeon_clear set clear_cnt=%d, update_time=now() where charac_no=%u",
        sig->m_subNo, sig->m_dungeonIdx);
    if (!db->exec(true))
        return 0;
    if (db->getAffectedRowCount() == 0)
    {
        db->set_query(
            "inSert into event_dungeon_clear(charac_no, clear_cnt, update_time) values(%u, %d, now())",
            sig->m_dungeonIdx, sig->m_subNo);
        if (!db->exec(true))
            return 0;
    }
    return 1;
