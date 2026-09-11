// ============================================================================
// DB_UpdateAutoMarketConditionsControl::dispatch —— ORIG 0x08433bb8（报告全文）
// ============================================================================
// DB_HANDLE_2。GetOutBuffer<SIG_UPDATE_AUTO_MARKET_CONDITIONS_CONTROL>
// （+0x00/+0x08/+0x10/+0x18=4×ull：total_gold/auction_gold/over_gold/
// optimum_gold_supply；+0x20/+0x24/+0x28=3×int phase）→ upDate daily 当日行
// （optimum=+0x18, over=+0x10, phase=+0x20/24/28）；exec 成功且
// getAffectedRowCount()==0 → inSert 当日行（total=+0x00, auction=+0x08,
// optimum=+0x18, over=+0x10, phase 同）；任一 exec 失败返 0。
// 无基类 dispatch 前置检查。
bool DB_UpdateAutoMarketConditionsControl::dispatch(int userIdx, int slot,
                                                    Stream* stream)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2,
                                                  E_SERVER_GROUP_NONE);
    SIG_UPDATE_AUTO_MARKET_CONDITIONS_CONTROL* sig =
        stream->GetOutBuffer<SIG_UPDATE_AUTO_MARKET_CONDITIONS_CONTROL>();
    db->set_query(
        "upDate  auto_market_condition_ctrl_daily set optimum_gold_supply=%qd, over_gold=%qd, gold_phase=%d, item_phase=%d, durability_phase=%d where occ_time = CURDATE()",
        sig->m_values0[3], sig->m_values0[2], sig->m_values20[0],
        sig->m_values20[1], sig->m_values20[2]);
    // ORIG +0xbc `xor $1;test;je body; mov $0;jmp end` = P16/P59 early-return
    // `if (!exec) return 0;`（与 BoosterGage 同形）。
    if (!db->exec(true))
        return 0;
    if (db->getAffectedRowCount() == 0)
    {
        db->set_query(
            "inSert into auto_market_condition_ctrl_daily(occ_time, total_gold, auction_gold, optimum_gold_supply, over_gold, gold_phase, item_phase, durability_phase) values(CURDATE(),%qd,%qd,%qd,%qd,%d,%d,%d)",
            sig->m_values0[0], sig->m_values0[1], sig->m_values0[3],
            sig->m_values0[2], sig->m_values20[0], sig->m_values20[1],
            sig->m_values20[2]);
        if (!db->exec(true))
            return 0;
    }
    return 1;
}