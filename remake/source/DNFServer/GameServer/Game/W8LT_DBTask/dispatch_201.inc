// ============================================================================
// DB_InsertLevelDungeonPlay::dispatch —— ORIG 0x0842cc98（报告 dispatch.md 全文）
// ============================================================================
// GetDBHandle(4,0) → *stream >> count(int)；循环 count 次：>> level、>>
// characCount、>> dungeonCount；upDate log_level_dungeon_play_count set
// total_charac_count=total_charac_count+%d,total_dungeon_play_count=
// total_dungeon_play_count+%d where level=%d（characCount,dungeonCount,level）；
// exec 失败或 affectedRowCount==0 → inSert ... values(%d,%d,%d)（level,
// characCount,dungeonCount），insert 失败返 0；循环完返 1。
// [b28d-solo] AE：P0 去具名 bool upOk（ORIG 无该槽往返 ⇒ 条件内联）；
// 4.1/4.4 逐条同形（原 -0xc 多占槽致全帧 -4 平移）；ORIG 0842cc98
bool DB_InsertLevelDungeonPlay::dispatch(int /*userIdx*/, int /*slot*/,
                                         Stream* stream)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_4,
                                                  E_SERVER_GROUP_NONE);
    int count = 0;         // -0x14
    int level = 0;         // -0x18
    int characCount = 0;   // -0x1c
    int dungeonCount = 0;  // -0x20
    *stream >> count;
    for (int i = 0; i < count; ++i)
    {
        *stream >> level;
        *stream >> characCount;
        *stream >> dungeonCount;
        db->set_query(
            "upDate log_level_dungeon_play_count set total_charac_count=total_charac_count+%d,total_dungeon_play_count=total_dungeon_play_count+%d where level=%d",
            characCount, dungeonCount, level);
        if (db->exec(true) != 1 || db->getAffectedRowCount() == 0)
        {
            db->set_query(
                "inSert into log_level_dungeon_play_count (level,total_charac_count,total_dungeon_play_count) values(%d,%d,%d)",
                level, characCount, dungeonCount);
            if (db->exec(true) != 1)
                return 0;
        }
    }
    return 1;
}