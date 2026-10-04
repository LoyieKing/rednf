// [b28d-4] DIFF：P183/P171：ORIG 多 {'mov': 6}（漏具名局部/去缓存不足）；ORIG 08444066
bool DB_StatisticCeraShopGiftLog::dispatch(int userIdx, int slot,
                                           Stream* stream)
{
    SIG_STATISTIC_CERASHOP_GIFT_LOG* sig = (
        stream->GetOutBuffer<SIG_STATISTIC_CERASHOP_GIFT_LOG>());
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_4,
                                                  E_SERVER_GROUP_NONE);
    if (db == 0)
        return 0;
    db->set_query(
        "inSert into log_cerashop_gift(occ_time, from_mid, from_charac_no, to_mid, to_charac_no, item_idx, item_cnt) values(unix_timestamp(), %u, %u, %u, %u, %u, %u)",
        sig->m_accId, sig->m_characNo, sig->m_buyIdx, sig->m_cost,
        sig->m_itemAddInfo, sig->m_itemAddInfo2);
    if (!db->exec(true))
    {
        // [SweepIntervalSet] P227 全表达式临时链：ORIG 0x08444137/0x0844414f 两处
        // 都用 ctor 返回的 this 调 operator()（`mov %eax,(%esp)`，非 lea 槽地址）；
        // 且第二处在 ctor **之前**把 6 个字段读入 %edi/%esi/%ebx + 3 个临时槽
        // （0x08444152..0x08444179），正是 6 个实参跨 ctor 保活的形。
        cMyTrace(__PRETTY_FUNCTION__, 0xa9c1, 5)(
            "DB_StatisticAmazingBoxProtectLucky_USE inSert exec fail");
        cMyTrace(__PRETTY_FUNCTION__, 0xa9c3, 5)(
            "inSert into log_cerashop_gift(occ_time, from_mid, from_charac_no, to_m_id, to_charac_no, item_idx, count) values(unix_timestamp(), %u, %u, %u, %u, %u, %u)",
            sig->m_accId, sig->m_characNo, sig->m_buyIdx,
            sig->m_cost, sig->m_itemAddInfo, sig->m_itemAddInfo2);
        return 0;
    }
    return 1;
}