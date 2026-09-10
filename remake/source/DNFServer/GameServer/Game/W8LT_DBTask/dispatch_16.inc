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
