int DB_LoadEtc::GetUserPvp(SIG_LOAD_ETC* sig)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)2,
                                                  (ENUM_SERVER_GROUP)0);
    db->set_query("seLect win,lose,pvp_point,pvp_grade,avg_kill_count,"
                  "avg_buf_count,avg_debuf_count,avg_heal_count,"
                  "avg_counter_count,avg_back_atk_count,avg_union_hit_count,"
                  "avg_overkill_count,avg_combo_count,avg_aerial_count,"
                  "avg_attacked_count,avg_deal_damage,avg_technic,avg_style,"
                  "avg_hit_penalty,pvp_count,win_point,pvp_grade_ext,"
                  "unix_timestamp(last_play_time),play_count,play_time,"
                  "unix_timestamp(pvp_grade_ext_update_time) "
                  "from pvp_result where charac_no=%u",
                  sig->m_characNo);
    if (!db->exec(true))
        return -1;
    if (!db->fetch())
        return 1;

    // sig+0x340 = m_pvpResult（PvpResultType，权威字段布局见 PvpResultType.h；
    // +0x0c/+0x10 两列 DB 不读）。ORIG 0x08407e45 `mov 0xc(%ebp),%eax;
    // add $0x340,%eax; mov %eax,-0x14(%ebp)` ⇒ 具名 PvpResultType* 局部
    // （P0 槽-局部对应律：有槽往返 ⇒ 原码有该局部），后续逐列 get_int（unrolled）：
    // win/lose/pvp_point→m_totalWinCount/m_totalLoseCount/m_expPoint，
    // pvp_grade→m_pvpGrade、avg[15]→m_avg[0..14]、pvp_count→m_count、
    // win_point→m_winPoint、pvp_grade_ext→m_todayWinCount。
    PvpResultType* pvp = &sig->m_pvpResult;
    if (!db->get_int(0, pvp->m_totalWinCount)) return -1;   // win
    if (!db->get_int(1, pvp->m_totalLoseCount)) return -1;  // lose
    if (!db->get_int(2, pvp->m_expPoint)) return -1;        // pvp_point
    if (!db->get_int(3, pvp->m_pvpGrade)) return -1;        // pvp_grade
    if (!db->get_int(4, pvp->m_avg[0])) return -1;
    if (!db->get_int(5, pvp->m_avg[1])) return -1;
    if (!db->get_int(6, pvp->m_avg[2])) return -1;
    if (!db->get_int(7, pvp->m_avg[3])) return -1;
    if (!db->get_int(8, pvp->m_avg[4])) return -1;
    if (!db->get_int(9, pvp->m_avg[5])) return -1;
    if (!db->get_int(10, pvp->m_avg[6])) return -1;
    if (!db->get_int(11, pvp->m_avg[7])) return -1;
    if (!db->get_int(12, pvp->m_avg[8])) return -1;
    if (!db->get_int(13, pvp->m_avg[9])) return -1;
    if (!db->get_int(14, pvp->m_avg[10])) return -1;
    if (!db->get_int(15, pvp->m_avg[11])) return -1;
    if (!db->get_int(16, pvp->m_avg[12])) return -1;
    if (!db->get_int(17, pvp->m_avg[13])) return -1;
    if (!db->get_int(18, pvp->m_avg[14])) return -1;
    if (!db->get_int(19, pvp->m_count)) return -1;          // pvp_count
    if (!db->get_int(20, pvp->m_winPoint)) return -1;       // win_point
    if (!db->get_int(21, pvp->m_todayWinCount)) return -1;  // pvp_grade_ext

    int lastPlaySec = 0;
    if (!db->get_int(22, lastPlaySec))              // unix(last_play_time)→局部
        return -1;
    // ORIG 0x08408273/0x084082a1：get_uint 直接传 pvp+0x64 / pvp+0x60（无中间局部）。
    if (!db->get_uint(23, (unsigned int&)pvp->m_todayPlayCount))
        return -1;
    if (!db->get_uint(24, (unsigned int&)pvp->m_todayLoseCount))
        return -1;

    time_t now = GlobalData::s_systemTime_.getCurSec();
    struct tm tmv;
    // ORIG 0x084082f0：localtime_r 返回值落具名 tm* 局部，成员经指针写（P43）。
    struct tm* t = localtime_r(&now, &tmv);
    // ORIG 0x084082f6..0x0840830a 写序 +0x8 → +0x4 → +0x0（tm_hour/tm_min/tm_sec）。
    t->tm_hour = 0;
    t->tm_min = 0;
    t->tm_sec = 0;
    // ORIG 0x0840831b：mktime 结果直存 +0x68（无 nextReset 局部），随后重读字段比较。
    pvp->m_nextResetTime = (int)mktime(t);
    if (pvp->m_nextResetTime > lastPlaySec)
        pvp->m_todayLoseCount = 0;                  // play_time 清零

    // ORIG 0x08408338 直接 lea -0x24（无前置清零）⇒ 裸声明。
    int gradeExtUpdateSec;
    if (!db->get_int(25, gradeExtUpdateSec))        // unix(pvp_grade_ext_update_time)
        return -1;
    // ORIG 0x08408360：getCurSec 返回值先落具名槽（mov %eax,-0xc）再与
    // gradeExtUpdateSec+0x15180 直跳比较（无 setcc 物化）。
    int nowSec = (int)GlobalData::s_systemTime_.getCurSec();
    if (gradeExtUpdateSec + 0x15180 < nowSec)
        pvp->m_todayWinCount = 0;                   // pvp_grade_ext 清零

    return 0;
