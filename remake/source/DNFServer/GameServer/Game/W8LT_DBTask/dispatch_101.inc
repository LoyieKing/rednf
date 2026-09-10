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
