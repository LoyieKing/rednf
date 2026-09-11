// ============================================================================
// DB_DeleteCharac::CheckEventCharacter40 @0x08403324
// charac_info 查 event_charac_level（m_id=accId 串、delete_flag=0）；
// exec/fetch/get_int 失败 → 0；level==0 → 0；否则 1。
// ============================================================================
bool DB_DeleteCharac::CheckEventCharacter40(SIG_DELETE_CHARAC* sig)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)2,
                                                  (ENUM_SERVER_GROUP)0);
    db->set_query("seLect event_charac_level from charac_info where m_id=%s "
                  "and charac_no=%d and delete_flag=0",
                  NumberToString(sig->m_accId, 0), sig->m_characNo);
    if (!db->exec(true))
    {
        return 0;
    }
    int level = 0;
    if (!db->fetch() || !db->get_int(0, level))
    {
        return 0;
    }
    if (level != 0)
    {
        return 1;
    }
    return 0;
}
