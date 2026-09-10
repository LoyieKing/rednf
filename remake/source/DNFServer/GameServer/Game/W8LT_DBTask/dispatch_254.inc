bool DB_EventAccountCharacterLevelUpOnceGift::dispatch(int userIdx, int slot,
                                                       Stream* stream)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_9,
                                                  E_SERVER_GROUP_NONE);
    unsigned int m_id = 0;
    short serverGroup = 0;
    int characNo = 0;
    short jobType = 0;
    short level = 0;
    *stream >> m_id;
    *stream >> serverGroup;
    *stream >> characNo;
    *stream >> jobType;
    *stream >> level;
    db->set_query(
        "inSert into event_1208_level_achieve(m_id ,server_id, charac_no, job_type, level, occ_date) values(%s, %d, %d, %d, %d, now())",
        NumberToString(m_id, 0), (int)serverGroup, characNo, (int)jobType,
        (int)level);
    if (!db->exec(true))
        return 0;
    return 1;
