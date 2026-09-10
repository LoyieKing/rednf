bool DB_SaveMercenaryData::dispatch(int /*userIdx*/, int /*slot*/,
                                    Stream* stream)
{
    // P89：单一视图表达式避免 raw/sig 双局部槽（ORIG 仅 sig(-0x24) 槽）。
    DB_MercenaryDataView* sig = reinterpret_cast<DB_MercenaryDataView*>(
        stream->GetOutBuffer<SIG_MERCENARY_DB_DATA>());
    if (sig == 0)
        return 0;
    if (sig->m_count < 1 || sig->m_count > 0x1e)
        return 0;
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2,
                                                  E_SERVER_GROUP_NONE);
    for (int i = 0; i < sig->m_count; ++i)
    {
        db->set_query(
            "upDate charac_link_bonus set mercenary_start_time=%d,mercenary_finish_time=%d,mercenary_area=%d,mercenary_period=%d where charac_no=%u",
            sig->m_rows[i].m_startTime,
            sig->m_rows[i].m_areaPeriod.m_finishTime,
            (int)sig->m_rows[i].m_areaPeriod.m_area,
            (int)sig->m_rows[i].m_areaPeriod.m_period,
            sig->m_rows[i].m_characNo);
        if (db->exec(true) != 1)
        {
            db->set_query(
                "inSert into charac_info(mercenary_start_time,mercenary_finish_time,mercenary_area,mercenary_period) values(%d,%d,%d,%d) where charac_no=%u",
                sig->m_rows[i].m_startTime,
                sig->m_rows[i].m_areaPeriod.m_finishTime,
                (int)sig->m_rows[i].m_areaPeriod.m_area,
                (int)sig->m_rows[i].m_areaPeriod.m_period,
                sig->m_rows[i].m_characNo);
            if (db->exec(true) != 1)
                return 0;
        }
    }
    return 1;
