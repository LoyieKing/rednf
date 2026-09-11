// ============================================================================
// DB_UpdateCharacPlayTime::dispatch —— ORIG 0x0843f75a（报告 dispatch.md 全文）
// ============================================================================
// ORIG 0x843f75a：charac_no=0/play_time=0 清零 → *stream >> charac_no(int) >>
// play_time(int) → GetDBHandle(DB_HANDLE_2,0) → upDate charac_stat set
// total_play_time=total_play_time + %d where charac_no = %d（%d=play_time、
// charac_no，与 SQL 占位序一致）→ exec(true)（返回值丢弃）→ 恒返 1。
// ============================================================================
bool DB_UpdateCharacPlayTime::dispatch(int /*userIdx*/, int /*slot*/,
                                       Stream* stream)
{
    MySQL* db;                 // -0xc（ORIG 最高槽，声明序在前）
    int characNo = 0;          // -0x10（SQL where charac_no=%d，第一读）
    int playTime = 0;          // -0x14（SQL +%d 增量，第二读）
    *stream >> characNo;
    *stream >> playTime;
    db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2,
                                           E_SERVER_GROUP_NONE);
    db->set_query(
        "upDate charac_stat set total_play_time=total_play_time + %d where charac_no = %d",
        playTime, characNo);
    db->exec(true);
    return 1;
}