int DB_SaveBloodDungeon::SaveBloodDungeonReward(SIG_BLOOD_DUNGEON* sig)
{
    // 声明序照 ORIG 栈槽（低→高）：tm(-0x54) today(-0x28) db(-0x24)
    // localTime(-0x20) weeklyOffset(-0x1c)。
    struct tm localTm;
    int today;
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2, E_SERVER_GROUP_NONE);
    struct tm* localTime;
    int weeklyOffset;

    today = getDailyInitializeTime(6, 0, 0);
    localTime = localtime_r((const time_t*)&today, &localTm);
    weeklyOffset = 0;   // ORIG movl $0,-0x1c（localtime_r 后、tm_wday 读取前）
    if (localTime->tm_wday == 0)
        weeklyOffset = 0x7e900;
    else
        weeklyOffset = (localTime->tm_wday - 1) * 0x15180;

    if (sig->m_flag1c != 0)
    {
        db->set_query("upDate charac_blood_dungeon_reward set reward=1 where "
                      "charac_no=%u and week_occ_date = "
                      "cast(from_unixtime(%d) as date)",
                      sig->m_characNo, (today - weeklyOffset) - 0x93a80);
        if (db->exec(true) != 1)
            return 0;
    }
    if (sig->m_field14 != 0)
    {
        db->set_query("upDate charac_blood_dungeon_reward set "
                      "week_point=%u,week_enter_count=%u,week_use_gold=%u,"
                      "last_play_date=cast(from_unixtime(%d) as date),"
                      "enter_count=%u where charac_no=%u and week_occ_date = "
                      "cast(from_unixtime(%d) as date)",
                      sig->m_point, sig->m_weeklyEnterCount, sig->m_useGold,
                      sig->m_dailyInitTime, sig->m_field14, sig->m_characNo,
                      today - weeklyOffset);
        if (db->exec(true) != 1)
            return 0;
        if (db->getAffectedRowCount() == 0)
        {
            db->set_query("inSert into charac_blood_dungeon_reward("
                          "charac_no,week_occ_date,week_point,week_enter_count,"
                          "week_use_gold,last_play_date,enter_count) "
                          "values(%u,cast(from_unixtime(%d) as date),%u,%u,%u,"
                          "cast(from_unixtime(%d) as date),%u)",
                          sig->m_characNo, today - weeklyOffset, sig->m_point,
                          sig->m_weeklyEnterCount, sig->m_useGold,
                          sig->m_dailyInitTime, sig->m_field14);
            if (db->exec(true) != 1)
                return 0;
        }
    }
    return 1;
