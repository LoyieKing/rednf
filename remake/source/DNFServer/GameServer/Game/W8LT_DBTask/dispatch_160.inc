bool DB_InsertDungeonEntrance::dispatch(int /*userIdx*/, int /*slot*/,
                                        Stream* stream)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_4,
                                                  E_SERVER_GROUP_NONE);
    int count = 0;              // -0x2c（循环次数，第一读）
    *stream >> count;
    int occDate = GlobalData::s_systemTime_.getCurSec();    // -0x24
    int channelNo = G_CEnvironment()->get_channel_no();     // -0x20
    int dungeonIndex = 0;       // -0x30
    int enterCount = 0;         // -0x34
    int exerciseCount = 0;      // -0x38
    // ORIG 0x08429696 执行序 = sql ctor(`lea -0x3c; call`) 在 rep stos 前；
    // 4.4 对 `{init}` 对象按声明逆序发射初始化 ⇒ buf 声明前置。
    char buf[0x400] = { '\0' }; // -0x43c..-0x3c（P4 rep stos 内联清零）
    std::string sql;            // -0x3c
    for (int i = 0; i < count; ++i)
    {
        *stream >> dungeonIndex;
        *stream >> enterCount;
        *stream >> exerciseCount;
        if (sql.size() != 0)
            sprintf(buf, ",(%d,from_unixtime(%d),%d,%d,%d)", channelNo,
                    occDate, dungeonIndex, enterCount, exerciseCount);
        else
            sprintf(buf, "(%d,from_unixtime(%d),%d,%d,%d)", channelNo,
                    occDate, dungeonIndex, enterCount, exerciseCount);
        sql += buf;
    }
    db->set_query(
        "inSert into log_dungeon_entrance(channel_no,occ_date,dungeon_index,enter_count, exercise_count) values%s",
        sql.c_str());
    // ORIG 0x08429696：尾 bool 物化双常数岛（xor$1;test;je + mov$0/mov$1
    // 入 %ebx）→ string dtor → 出口；catch 块置 dtor 前（4.1 EH 布局）。
    // 直判返形先例：DB_InsertPVPMap/DB_InsertDungeonExp 闭合。
    if (!db->exec(true))
        return 0;
    return 1;
