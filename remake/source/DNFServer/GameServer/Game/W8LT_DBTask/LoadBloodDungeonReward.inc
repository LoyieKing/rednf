int DB_LoadNoCacheData::LoadBloodDungeonReward(SIG_LOAD_NO_CACHE_DATA* sig)
{
    // 声明序照 ORIG 栈槽（低→高）：tm(-0x5c) rewardByte(-0x29) today(-0x28)
    // db(-0x24) localTime(-0x20) weeklyOffset(-0x1c) nRows(-0x18)
    // colIdx(-0x14) mktimeVal(-0x10) colIdx2(-0xc)。
    struct tm localTm;
    int today;
    char rewardByte;
    MySQL* db;
    struct tm* localTime;
    int weeklyOffset;
    int nRows;
    int colIdx;
    time_t mktimeVal;
    int colIdx2;

    db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2, E_SERVER_GROUP_NONE);
    today = getDailyInitializeTime(6, 0, 0);
    localTime = localtime_r((const time_t*)&today, &localTm);
    weeklyOffset = 0;
    if (localTime->tm_wday == 0)
        weeklyOffset = 0x7e900;
    else
        weeklyOffset = (localTime->tm_wday - 1) * 0x15180;

    db->set_query(
        "seLect week_point,week_enter_count,week_use_gold,unix_timestamp(last_play_date),enter_count,rank,reward,reward_item_id,reward_gold from charac_blood_dungeon_reward where charac_no=%d and week_occ_date = cast(from_unixtime(%d) as date)",
        ((DB_LoadNoCacheDataView*)sig)->m_characNo, today - weeklyOffset);
    if (db->exec(true) != 1)
        return 0;
    nRows = db->get_n_rows();
    if (nRows == 1)
    {
        if (db->fetch() != 1)
            return 0;
        colIdx = 0;
        if (db->get_uint(colIdx++, ((DB_LoadNoCacheDataView*)sig)->m_weekPoint) != 1)
            return 0;
        if (db->get_uint(colIdx++, ((DB_LoadNoCacheDataView*)sig)->m_weekEnterCount) != 1)
            return 0;
        if (db->get_uint(colIdx++, ((DB_LoadNoCacheDataView*)sig)->m_weekUseGold) != 1)
            return 0;
        if (db->get_int(colIdx++, ((DB_LoadNoCacheDataView*)sig)->m_lastPlayDate) != 1)
            return 0;
        if (db->get_uint(colIdx++, ((DB_LoadNoCacheDataView*)sig)->m_enterCount) != 1)
            return 0;
        localTime->tm_hour = 0;
        localTime->tm_min = 0;
        localTime->tm_sec = 0;
        mktimeVal = mktime(localTime);
        if (((DB_LoadNoCacheDataView*)sig)->m_lastPlayDate != (int)mktimeVal)
        {
            ((DB_LoadNoCacheDataView*)sig)->m_enterCount = 0;
            ((DB_LoadNoCacheDataView*)sig)->m_lastPlayDate = (int)mktimeVal;
        }
    }
    db->set_query(
        "seLect rank,reward,reward_item_id,reward_gold from charac_blood_dungeon_reward where charac_no=%d and week_occ_date = cast(from_unixtime(%d) as date)",
        ((DB_LoadNoCacheDataView*)sig)->m_characNo,
        (today - weeklyOffset) - 0x93a80);
    if (db->exec(true) != 1)
        return 0;
    nRows = db->get_n_rows();
    if (nRows == 1)
    {
        if (db->fetch() != 1)
            return 0;
        colIdx2 = 0;
        if (db->get_uint(colIdx2++, ((DB_LoadNoCacheDataView*)sig)->m_rank) != 1)
            return 0;
        rewardByte = 0;
        if (db->get_byte(colIdx2++, rewardByte) != 1)
            return 0;
        // 字段区见 DB_LoadNoCacheDataView::m_reward（+0x20）。gcc 对
        // reinterpret_cast 视图 lvalue 的求值点与 ORIG（rhs 先行、sig 基址
        // 后取）不符，保留头 m_pad 下标形态以逐指令一致（§4.3 例外注记）。
        sig->m_pad[0x20] = (char)(rewardByte == 0);
        if (db->get_ulong(colIdx2++, ((DB_LoadNoCacheDataView*)sig)->m_rewardItemId) != 1)
            return 0;
        if (db->get_uint(colIdx2++, ((DB_LoadNoCacheDataView*)sig)->m_rewardGold) != 1)
            return 0;
    }
    return 1;
