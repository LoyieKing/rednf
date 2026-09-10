bool DB_EventUpdateGiveGrowCreature::dispatch(int userIdx, int slot,
                                              Stream* stream)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_9,
                                                  E_SERVER_GROUP_NONE);
    unsigned int m_id = 0;
    *stream >> m_id;
    SIG_GIVE_GROWTH_CREATURE_EVENT::eventCharacInfo* sig =
        stream->GetOutBuffer<SIG_GIVE_GROWTH_CREATURE_EVENT::eventCharacInfo>();
    db->set_query(
        "upDate event_1208_growthcreature set charac_name = %s) where m_id = %s and server_id =%d and charac_no = %d",
        sig->m_name, NumberToString(m_id, 0),
        (int)(signed short)sig->m_serverGroup, sig->m_characNo);
    if (!db->exec(true))
        return 0;
    return 1;
