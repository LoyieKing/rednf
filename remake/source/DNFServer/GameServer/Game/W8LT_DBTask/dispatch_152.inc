bool DB_InsertUserPlayTime::dispatch(int /*userIdx*/, int /*slot*/, Stream* stream)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_6,
                                                  E_SERVER_GROUP_NONE);   // -0x1c
    unsigned int mId = 0;      // -0x20
    int playTime = 0;          // -0x24
    int tradeCnt = 0;          // -0x28
    unsigned int exp = 0;      // -0x2c
    unsigned short usedFatigue = 0;   // -0x2e（movw 清零）
    bool tingCount;            // -0x2f（被 ushort 清零顺带覆盖，不单独初始化）
    stream->operator>>(mId);
    stream->operator>>(playTime);
    stream->operator>>(tradeCnt);
    stream->operator>>(exp);
    stream->operator>>(usedFatigue);
    stream->operator>>(tingCount);
    char tableName[0x14];      // -0x43
    memset(tableName, 0, 0x14);
    memcpy(tableName, "member_play_info", 0x11);
    db->set_query(
        "upDate %s set play_time=play_time+%d,play_count=play_count+1,trade_cnt=trade_cnt+%d, exp=exp+%u, used_fatigue=used_fatigue+%u, ting_count=ting_count+%d where occ_date=cast(now() as date) and m_id=%s",
        tableName, playTime, tradeCnt, exp, (unsigned int)usedFatigue,
        (unsigned int)(tingCount ? 0 : 1), NumberToString(mId, 0));
    if (!(db->exec(true) == 1 && db->getAffectedRowCount() != 0))
    {
        db->set_query(
            "inSert into %s(occ_date,m_id,play_time,play_count,trade_cnt,exp,used_fatigue,ting_count) values(cast(now() as date),%s,%d,1,%d,%u,%u,%d)",
            tableName, NumberToString(mId, 0), playTime, tradeCnt, exp,
            (unsigned int)usedFatigue, (unsigned int)(tingCount ? 0 : 1));
        if (db->exec(true) != 1)
            return 0;
    }
    return 1;
