bool DB_UpdateQueryCountDbLogdb::dispatch(int /*userIdx*/, int /*slot*/,
                                          Stream* stream)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_4,
                                                  E_SERVER_GROUP_NONE);
    int occTime = 0;    // -0x14
    int count = 0;      // -0x18
    *stream >> occTime;
    *stream >> count;
    int i = 0;          // -0xc（while 形态：先 jmp 到比较）
    while (i < count)
    {
        int qId = 0;            // -0x1c（解包 1 → SQL q_id）
        int total = 0;          // -0x20（解包 2 → SQL total）
        int responseTime = 0;   // -0x24（解包 3 → SQL response_time）
        *stream >> qId;
        *stream >> total;
        *stream >> responseTime;
        db->set_query(
            "inSert into log_query_stat(occ_time,q_id,total,response_time,gc_no) "
            "values(from_unixtime(%d),%d,%d,%d,%d)",
            occTime, qId, total, responseTime,
            G_CEnvironment()->get_channel_no());
        if (db->exec(true) != 1)
            return 0;
        ++i;
    }
    return 1;
