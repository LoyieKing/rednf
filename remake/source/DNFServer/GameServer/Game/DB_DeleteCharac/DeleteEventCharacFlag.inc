bool DB_DeleteCharac::DeleteEventCharacFlag(SIG_DELETE_CHARAC* sig)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)6,
                                                  (ENUM_SERVER_GROUP)0);
    db->set_query("upDate member_login set event_charac_flag = 0 where m_id=%s",
                  NumberToString(sig->m_accId, 0));
    if (!db->exec(true))
    {
        return 0;
    }
    return 1;
