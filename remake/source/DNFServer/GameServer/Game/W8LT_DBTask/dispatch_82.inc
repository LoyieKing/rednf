bool DB_UpdateCharacterDay::dispatch(int userIdx, int slot, Stream* stream)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_6,
                                                  E_SERVER_GROUP_NONE);
    DB_UpdateCharacterDayView* sig = reinterpret_cast<DB_UpdateCharacterDayView*>(
        stream->GetOutBuffer<SIG_UPDATE_CHARACTER_DAY_EVENT>());
    db->set_query(
        "upDate event_1111_charac_day set entries_count=entries_count+%u, update_time=now() where m_id=%s",
        (unsigned int)sig->m_entries, NumberToString(sig->m_mId, 0));
    if (db->exec(true) != 1)
        return 0;
    if (db->getAffectedRowCount() == 0)
    {
        db->set_query(
            "inSert into event_1111_charac_day(m_id, entries_count, update_time) values(%s, %u, now())",
            NumberToString(sig->m_mId, 0), (unsigned int)sig->m_entries);
        if (db->exec(true) != 1)
            return 0;
    }
    return 1;
