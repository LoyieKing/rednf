bool DB_InsertResponseTime::dispatch(int /*userIdx*/, int /*slot*/, Stream* stream)
{
    int i = 0;                      // -0x1c
    SIG_INSERT_RESPONSE_TIME* sig;  // -0x20
    MySQL* db;                      // -0x24
    std::string values;             // -0x28
    char monthBuf[10] = { '\0' };   // -0x32（ORIG int/int/short 三段清零）
    time_t occTime;                 // -0x38
    struct tm tmBuf;                // -0x64
    char rowBuf[0xff] = { '\0' };   // -0x163（ORIG 255 字节带对齐分支 rep stos）
    sig = stream->GetOutBuffer<SIG_INSERT_RESPONSE_TIME>();
    db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_4,
                                           E_SERVER_GROUP_NONE);
    while (i <= 99 && i < ((DB_ResponseTimeSigView*)sig)->m_count)
    {
        if (!values.empty())
            values += ",";
        // ORIG 行字段逐次重读（P48 无 CSE）；%llu 直接读 8 字节（低字先行）。
        sprintf(rowBuf, "(%d,from_unixtime(%d),%u,%u,%llu,%u)",
                G_CEnvironment()->get_channel_no(),
                ((DB_ResponseTimeSigView*)sig)->m_baseSec,
                ((DB_ResponseTimeSigView*)sig)->m_entries[i].m_id,
                ((DB_ResponseTimeSigView*)sig)->m_entries[i].m_count,
                *(unsigned long long*)&
                    ((DB_ResponseTimeSigView*)sig)->m_entries[i].m_bytesLow,
                ((DB_ResponseTimeSigView*)sig)->m_entries[i].m_avg);
        values += rowBuf;
        ++i;
    }
    occTime = (time_t)((DB_ResponseTimeSigView*)sig)->m_baseSec;
    localtime_r(&occTime, &tmBuf);
    strftime(monthBuf, 10, "%Y%m", &tmBuf);
    db->set_query(
        "inSert into log_response_time_%s(channel_no,occ_time,packet_id,packet_count,total_response_time,avg_response_time) values%s",
        monthBuf, values.c_str());
    return db->exec(true);
