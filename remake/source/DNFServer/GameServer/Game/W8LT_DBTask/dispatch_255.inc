bool DB_EventSaveGiveGrowCreature::dispatch(int userIdx, int slot,
                                            Stream* stream)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_9,
                                                  E_SERVER_GROUP_NONE);
    unsigned int m_id = 0;
    *stream >> m_id;
    SIG_GIVE_GROWTH_CREATURE_EVENT::eventCharacInfo* sig =
        stream->GetOutBuffer<SIG_GIVE_GROWTH_CREATURE_EVENT::eventCharacInfo>();
    db->set_query(
        "inSert into event_1208_growthcreature(m_id ,server_id, charac_no, job_type, charac_name, occ_date) values(%s, %d, %d, %d, '%s', now())",
        // ORIG 实参序：server_id=(int)(short)+0x0（ebx 落 0xc）、charac_no=+0x4、
        // job_type=+0x8 int（右起先求值 edi←mov 0x8）；此前误将 m_job 置于 server_id。
        NumberToString(m_id, 0), (int)(signed short)sig->m_serverGroup,
        sig->m_characNo, sig->m_job, sig->m_name);
    if (!db->exec(true))
        return 0;
    return 1;
