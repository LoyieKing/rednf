bool DB_DeleteCharac::DeleteCharac(SIG_DELETE_CHARAC* sig)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)2,
                                                  (ENUM_SERVER_GROUP)0);
    db->set_query("upDate charac_info set delete_time=now(),delete_flag=1 "
                  "where charac_no=%u and m_id=%s", sig->m_characNo,
                  NumberToString(sig->m_accId, 0));
    if (!db->exec(true))
    {
        return 0;
    }

    MySQL* db2 = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)1,
                                                   (ENUM_SERVER_GROUP)0);
    db2->set_query("deLete from dnf_master_charac where m_id='%u' and "
                   "server_id='%u' and charac_no='%u'", sig->m_accId,
                   G_CEnvironment()->m_serverEnvir.m_serverGroup,
                   sig->m_characNo);
    db2->exec(true);
    return 1;
