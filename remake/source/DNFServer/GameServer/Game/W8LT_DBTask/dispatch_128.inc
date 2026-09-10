bool DB_InsertDungeonExp::dispatch(int userIdx, int slot, Stream* stream)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_4,
                                                  E_SERVER_GROUP_NONE);
    int curSec = GlobalData::s_systemTime_.getCurSec();
    int lev = 0;
    int exp = 0;
    int fatigue = 0;
    int userCount = 0;
    std::string sql;
    int count = 0;
    char buf[1024];
    *stream >> count;
    for (int i = 0; i < count; ++i)
    {
        *stream >> lev;
        *stream >> exp;
        *stream >> fatigue;
        *stream >> userCount;
        sprintf(buf, "(from_unixtime(%d),%d,%d,%d,%d,%d)", curSec,
                G_CEnvironment()->get_channel_no(), lev, exp, fatigue,
                userCount);
        sql += buf;
        if (i < count - 1)
            sql += ",";
    }
    db->set_query(
        "inSert into log_dungeon_exp(occ_time,channel_no,lev,exp,fatigue,user_count) values%s",
        sql.c_str());
    // ORIG 0x0842e944：同 DB_InsertPVPMap 尾——bool 物化双常数岛
    // （`xor$1;test;je` + mov$0/mov$1 入 %ebx）后走 string dtor；
    // 4.4 直判返形复现（先例 DB_InsertPVPMap::dispatch 闭合）。
    if (!db->exec(true))
        return 0;
    return 1;
