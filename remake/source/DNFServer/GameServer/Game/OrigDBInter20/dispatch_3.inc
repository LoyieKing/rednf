bool DB_SecurityCardUpdateValidityTime::dispatch(int userIdx, int slot,
                                                 Stream* stream)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_6,
                                                  E_SERVER_GROUP_NONE);
    SIG_SECURITY_CARD_UPDATE_VALIDITYTIME* sig =
        stream->GetOutBuffer<SIG_SECURITY_CARD_UPDATE_VALIDITYTIME>();

    db->set_query(
        "upDate member_security_card set validity_time=unix_timestamp(now())+%d where m_id=%s and apply_flag=1",
        sig->m_flag,
        NumberToString(
            sig->m_mId, 0));
    db->exec(true);
    return 1;
}