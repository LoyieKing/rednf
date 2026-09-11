// ============================================================================
// DB_InsertPVPMap::dispatch —— ORIG 0x0842cb12
// ============================================================================
// DB_InsertPVPMap::dispatch —— ORIG 0x0842cb12
// ============================================================================
// ORIG 0x842cb12：GetDBHandle(DB_HANDLE_4,0) → count/mapId/playCount=0 →
// string ctor → Stream>>count → 循环 count 次：Stream>>mapId、Stream>>
// playCount → sprintf(local_424,"(%d,now(),%d,%d)",channel_no,mapId,
// playCount)（channel_no=G_CEnvironment()->get_channel_no()）→ string +=
// 缓冲；非末项 string += ","。循环后
// set_query("inSert into log_pvp_map(channel_no,occ_date,map_id,play_count)
// values%s", string.c_str()) → exec(true) → return exec==1。
// 异常路径（C++98 try/catch 由 ORIG 编译器生成）：string dtor 后 rethrow。
bool DB_InsertPVPMap::dispatch(int userIdx, int slot, Stream* stream)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_4,
                                                  E_SERVER_GROUP_NONE);
    int count = 0;
    int mapId = 0;
    int playCount = 0;
    std::string sql;
    char buf[1024];
    *stream >> count;
    for (int i = 0; i < count; ++i)
    {
        *stream >> mapId;
        *stream >> playCount;
        sprintf(buf, "(%d,now(),%d,%d)", G_CEnvironment()->get_channel_no(),
                mapId, playCount);
        sql += buf;
        if (i < count - 1)
            sql += ",";
    }
    db->set_query(
        "inSert into log_pvp_map(channel_no,occ_date,map_id,play_count) values%s",
        sql.c_str());
    // ORIG 0x842…：返回值经 `xor$1;test;je` 双常数物化入 %ebx 再走
    // string dtor 尾（4.1 bool 物化形）。4.4 对 `== 1` 恒折裸值直通
    // ⇒ 试直判返形（技巧1/17 系）。
    if (!db->exec(true))
        return 0;
    return 1;
}