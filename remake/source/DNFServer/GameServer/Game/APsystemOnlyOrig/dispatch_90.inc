// ============================================================================
// DB_SaveChannelUserLevel::dispatch —— ORIG 0x0841be50（报告 dispatch.md 全文）
// ============================================================================
// 无基类前置。GetOutBuffer<SIG_UPDATE_STATISTIC4LEV> → GetDBHandle(4,0) →
// lev=1..0x46 循环 inSert channel_lev_status：
//   values(from_unixtime(%d),%d,%d,%d) = m_time, m_gcNo, lev, m_count[lev]；
// 任一 exec 失败返 0，循环完成返 1。
bool DB_SaveChannelUserLevel::dispatch(int userIdx, int slot, Stream* stream)
{
    // 声明序照 ORIG 栈槽（低→高）：sig(-0x14) db(-0x10) lev(-0xc)。
    SIG_UPDATE_STATISTIC4LEV* sig =
        stream->GetOutBuffer<SIG_UPDATE_STATISTIC4LEV>();
    MySQL* db =
        GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_4, E_SERVER_GROUP_NONE);
    for (int lev = 1; lev <= 0x46; ++lev)
    {
        db->set_query(
            "inSert into channel_lev_status(occ_time,channel_no,lev,occ_count) values(from_unixtime(%d),%d,%d,%d)",
            sig->m_time, sig->m_gcNo, lev, sig->m_count[lev]);
        if (db->exec(true) != 1)
            return 0;
    }
    return 1;
}