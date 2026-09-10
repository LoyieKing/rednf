bool DB_GoblinPadUpdateValidityTime::dispatch(int /*userIdx*/, int /*slot*/,
                                              Stream* stream)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_6,
                                                  E_SERVER_GROUP_NONE);
    SIG_GOBLIN_UPDATE_VALIDITYTIME* sig =
        stream->GetOutBuffer<SIG_GOBLIN_UPDATE_VALIDITYTIME>();
    db->set_query(
        "upDate member_mousepass set validity_time=unix_timestamp(now())+%d where m_id=%s",
        sig->m_flag,
        NumberToString(sig->m_mId, 0));
    db->exec(true);
    return 1;
