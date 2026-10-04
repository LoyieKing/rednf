// ============================================================================
// DB_SaveMercenaryData::dispatch —— ORIG 0x084458c6（报告 dispatch.md 全文）
// ============================================================================
// GetOutBuffer<SIG_MERCENARY_DB_DATA>；空返 0；+0x00 count 越界（<1 或
// >0x1e）返 0。GetDBHandle(DB_HANDLE_2,0) → 循环 count 次：行步长 0x54，
// +0x04 charac_no、+0x2c mercenary_start_time、+0x30 mercenary_finish_time、
// +0x34 area(char)、+0x35 period(char)。set_query("upDate charac_link_bonus
// set mercenary_start_time=%d,mercenary_finish_time=%d,mercenary_area=%d,
// mercenary_period=%d where charac_no=%u", start, finish, area, period,
// charac_no)；exec!=1 → 改 set_query("inSert into charac_info(...,) values(
// %d,%d,%d,%d) where charac_no=%u" 同 5 参)，exec!=1 → 返 0；循环完返 1。
// ============================================================================
// [b28d-4] DIFF：N82 槽号错位/多局部残差（4.1 不做 lifetime 槽复用）；ORIG 084458c6
bool DB_SaveMercenaryData::dispatch(int /*userIdx*/, int /*slot*/,
                                    Stream* stream)
{
    // P89：单一视图表达式避免 raw/sig 双局部槽（ORIG 仅 sig(-0x24) 槽）。
    SIG_MERCENARY_DB_DATA* sig =
        stream->GetOutBuffer<SIG_MERCENARY_DB_DATA>();
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
            sig->m_characs[i].m_startTime,
            sig->m_characs[i].m_finishTime,
            (int)sig->m_characs[i].m_area,
            (int)sig->m_characs[i].m_period,
            sig->m_characs[i].m_characNo);
        if (db->exec(true) != 1)
        {
            db->set_query(
                "inSert into charac_info(mercenary_start_time,mercenary_finish_time,mercenary_area,mercenary_period) values(%d,%d,%d,%d) where charac_no=%u",
                sig->m_characs[i].m_startTime,
                sig->m_characs[i].m_finishTime,
                (int)sig->m_characs[i].m_area,
                (int)sig->m_characs[i].m_period,
                sig->m_characs[i].m_characNo);
            if (db->exec(true) != 1)
                return 0;
        }
    }
    return 1;
}