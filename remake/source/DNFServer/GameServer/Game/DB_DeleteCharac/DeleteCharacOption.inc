bool DB_DeleteCharac::DeleteCharacOption(SIG_DELETE_CHARAC* sig)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)2,
                                                  (ENUM_SERVER_GROUP)0);
    db->set_query("deLete from charac_option where charac_no = %u",
                  sig->m_characNo);
    if (!db->exec(true))
    {
        return 0;
    }
    return 1;
}
