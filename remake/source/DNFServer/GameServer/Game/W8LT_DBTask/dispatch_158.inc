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
