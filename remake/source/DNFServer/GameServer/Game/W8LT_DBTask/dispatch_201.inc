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
