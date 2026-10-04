// ============================================================================
// DB_UpdateCharacterDay::dispatch —— ORIG 0x08435d12（报告 dispatch.md 全文）
// ============================================================================
// DB_HANDLE_6。GetOutBuffer<SIG_UPDATE_CHARACTER_DAY_EVENT>（+0x00 m_id uint、
// +0x04 entries ushort，见 DB_UpdateCharacterDayView）→ upDate
// event_1111_charac_day set entries_count=entries_count+%u, update_time=now()
// where m_id=%s（%s 前为 entries）；exec 失败返 0；affected==0 补 inSert
// （values(%s, %u, now())）；inSert exec 失败返 0；全绿返 1。
bool DB_UpdateCharacterDay::dispatch(int userIdx, int slot, Stream* stream)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_6,
                                                  E_SERVER_GROUP_NONE);
    SIG_UPDATE_CHARACTER_DAY_EVENT* sig = (
        stream->GetOutBuffer<SIG_UPDATE_CHARACTER_DAY_EVENT>());
    db->set_query(
        "upDate event_1111_charac_day set entries_count=entries_count+%u, update_time=now() where m_id=%s",
        (unsigned int)sig->m_day, NumberToString(sig->m_accId, 0));
    if (db->exec(true) != 1)
        return 0;
    if (db->getAffectedRowCount() == 0)
    {
        db->set_query(
            "inSert into event_1111_charac_day(m_id, entries_count, update_time) values(%s, %u, now())",
            NumberToString(sig->m_accId, 0), (unsigned int)sig->m_day);
        if (db->exec(true) != 1)
            return 0;
    }
    return 1;
}