// ORIG 0x0842945e（_ZN23DB_InsertValueStatistic8dispatchEiiP6Stream）：
// GetDBHandle(4,0)；GetOutBuffer<SIG_VALUE_STATISTIC>；按 +0x00..+0x68
// 28 个 uint（SQL 参数序 +0x00(occ_time from_unixtime) → +0x68 末列）；
// inSert log_value_stat(...28 列...)；返 exec(true)==1。
// [b28d-4] DIFF：N18a/P211：4.4 多 {'mov': 3} 中转 mov（单段直落/同型化待验）；ORIG 0842945e
bool DB_InsertValueStatistic::dispatch(int userIdx, int slot, Stream* stream)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_4,
                                                  E_SERVER_GROUP_NONE);
    SIG_VALUE_STATISTIC* sig = stream->GetOutBuffer<SIG_VALUE_STATISTIC>();
    // [H4 2026-09-11 D 组取证] ORIG 0x08429587 无 channelNo 具名槽（`call
    // G_CEnvironment; mov %eax,(%esp); call get_channel_no; mov %eax,0x8(%esp)`
    // ——值直落出参槽且**最后**求值，P329 实参位内联）；另 ORIG 只传 27 个 sig
    // 字段（装载偏移 +0x68→+0x00 ⇒ m_stats[0..26]），我方多传 m_stats[27]（+0x6c）。
    db->set_query(
        "inSert into log_value_stat(channel_no,occ_time ,level,uv,drop_gold,drop_item,result_card_gold,result_card_item,gold_card_item,store_item_buy,jar_item,disjoint_create,upgrade_faild_forced_disjoint,quest_reward,deathtower_card_gold,deathtower_card_item,consume_store_item_buy,consume_upgrade_attempt,consume_upgrade_faild,consume_stamina_recovery,consume_quest_consume,consume_auction_commision,consume_item_disjoint,consume_item_repair,consume_item_use,consume_item_drop,consume_gold_drop,consume_gold_card_price) values(%d,from_unixtime(%d),%d,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u)",
        G_CEnvironment()->get_channel_no(), sig->m_stats[0],
        sig->m_stats[1], sig->m_stats[2],
        sig->m_stats[3], sig->m_stats[4],
        sig->m_stats[5], sig->m_stats[6],
        sig->m_stats[7], sig->m_stats[8],
        sig->m_stats[9], sig->m_stats[10],
        sig->m_stats[11], sig->m_stats[12],
        sig->m_stats[13], sig->m_stats[14],
        sig->m_stats[15], sig->m_stats[16],
        sig->m_stats[17], sig->m_stats[18],
        sig->m_stats[19], sig->m_stats[20],
        sig->m_stats[21], sig->m_stats[22],
        sig->m_stats[23], sig->m_stats[24],
        sig->m_stats[25], sig->m_stats[26]);
    if (!db->exec(true))
        return 0;
    return 1;
}