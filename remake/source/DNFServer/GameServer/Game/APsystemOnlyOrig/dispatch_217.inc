// ============================================================================
// DB_LogIngameAdvertisement::dispatch —— ORIG 0x0844c304（报告 dispatch.md
// 全文）
// ============================================================================
// GetDBHandle(4,0) → GetOutBuffer<SIG_IN_GAME_ADVERTISEMENT_LOG>（null 返
// 0）→ inSert log_in_game_ad 10 列（state=+0x08 字节、view_code=+0x0a short、
// type_code=+0x0c 字节、banner_id=+0x10、dungeon/village/area=+0x14/18/1c）；
// 返 exec==1。
bool DB_LogIngameAdvertisement::dispatch(int /*userIdx*/, int /*slot*/,
                                         Stream* stream)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_4,
                                                  E_SERVER_GROUP_NONE);
    SIG_IN_GAME_ADVERTISEMENT_LOG* sig =
        stream->GetOutBuffer<SIG_IN_GAME_ADVERTISEMENT_LOG>();
    if (sig == 0)
        return 0;
    bool ok;   // ORIG 0x0844…：exec 结果物化 byte 槽 -0x19
    db->set_query(
        "inSert into log_in_game_ad(occ_date, m_id, charac_no, state, view_code, type_code, banner_id, dungeon_index, village_index, area_index) values (now(), %u, %u, %d, %d, %d, %u, %d, %d, %d)",
        sig->m_accId, sig->m_characNo, (int)(char)sig->m_type,
        (int)sig->m_adType, (int)(char)sig->m_flag, sig->m_extra,
        sig->m_dungeonIdx, sig->m_village, sig->m_pad1c);
    ok = db->exec(true);
    if (!ok)
        return 0;
    return 1;
}