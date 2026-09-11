// ============================================================================
// DB_InsertDungeonExp::dispatch —— ORIG 0x0842e944（报告 dispatch.md 全文）
// ============================================================================
// DB_HANDLE_4。curSec=GlobalData::s_systemTime_.getCurSec() → *stream >>
// count → 循环 count 次：*stream >> lev >> exp >> fatigue >> user_count
// （4×int）→ sprintf(buf,"(from_unixtime(%d),%d,%d,%d,%d,%d)", curSec,
// G_CEnvironment()->get_channel_no(), lev, exp, fatigue, user_count) →
// std::string += buf；非末项 += ","。循环后 set_query("inSert into
// log_dungeon_exp(occ_time,channel_no,lev,exp,fatigue,user_count) values%s",
// sql.c_str()) → 返回 exec(true)==1。
// ============================================================================
// ORIG 帧（gcc4.1）：db(-0x24)/curSec(-0x20)/v1..v4(-0x28..-0x34)/
// sql(-0x38)/count(-0x3c)/i(-0x1c)/buf(-0x448)。声明序照 ORIG 槽序。
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
}