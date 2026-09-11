// ORIG 0x08446196 sz 0x1a9：GetDBHandle(4,0) → 解包 m_id(uint)/charac_no(int)/
// start_time(int)/finish_time(int)/type(char)/period(char)/area(char)/gold(int)/
// item_id(int)/rarity(int) → NumberToString(m_id,0) → inSert into
// log_mercenary(m_id,charac_no,start_time,finish_time,type,period,area,gold,
// item_id,rarity) values(%s,%u,from_unixtime(%d),from_unixtime(%d),%d,%d,%d,
// %d,%d,%d) → 返回 exec(true)==1。
// [b28d-solo-2R] AE：P309 声明序翻案——char 组(type/period/area)由 int 组中间移到最后声明，
//   4.4 即复现 ORIG 的深槽共享布局（char 三字节落 -0x3b..-0x39，int 组 -0x20..-0x38 全等）；
//   原 NEAR(12) 槽号差全消。ORIG 0x8446196
bool DB_MercenaryLogInfo::dispatch(int userIdx, int slot, Stream* stream)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_4,
                                                  E_SERVER_GROUP_NONE);
    unsigned int mId;
    int characNo;
    int startTime;
    int finishTime;
    int gold;
    int itemId;
    int rarity;
    char type;
    char period;
    char area;

    *stream >> mId;
    *stream >> characNo;
    *stream >> startTime;
    *stream >> finishTime;
    *stream >> type;
    *stream >> period;
    *stream >> area;
    *stream >> gold;
    *stream >> itemId;
    *stream >> rarity;
    db->set_query("inSert into log_mercenary(m_id,charac_no,start_time,finish_time,type,period,area,gold,item_id,rarity) values(%s,%u,from_unixtime(%d),from_unixtime(%d),%d,%d,%d,%d,%d,%d)",
                  NumberToString(mId, 0), characNo, startTime, finishTime,
                  (int)type, (int)period, (int)area, gold, itemId, rarity);
    // ORIG 尾块 xor $1/test 物化（gcc4.1 `if(!call())return 0;return 1`）。
    if (!db->exec(true))
        return 0;
    return 1;
}