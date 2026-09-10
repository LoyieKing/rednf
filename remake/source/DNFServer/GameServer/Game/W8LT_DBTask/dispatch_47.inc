bool DB_UpdatePvPResult::dispatch(int userIdx, int slot, Stream* stream)
{
    // 局部类 school_info（ORIG W C1 0x0843091e；9 字段全清零）
    struct school_info
    {
        school_info()
        {
            m_schoolId = 0;
            m_fightCnt = 0;
            m_lastUpdateTime = 0;
            m_fightCntCur = 0;
            m_schoolPoint = 0;
            m_schoolPointPrev = 0;
            m_fightCntPrev = 0;
            m_rating = 0;
            m_ratingPrev = 0;
        }
        int m_schoolId;          // +0x00
        int m_fightCnt;          // +0x04
        int m_lastUpdateTime;    // +0x08
        int m_fightCntCur;       // +0x0c
        int m_schoolPoint;       // +0x10
        int m_schoolPointPrev;   // +0x14
        int m_fightCntPrev;      // +0x18
        float m_rating;          // +0x1c
        float m_ratingPrev;      // +0x20
    };

    school_info row;        // DB 查询主行（ORIG 槽 -0x94）
    school_info rows[2];    // 两队槽（ORIG 槽 -0x108..-0xc0）
    int curSec = GlobalData::s_systemTime_.getCurSec();
    struct tm tmval;
    struct tm* ptm = localtime_r((const time_t*)&curSec, &tmval);
    int wday = ptm->tm_wday;
    int mday = ptm->tm_mday;
    ptm->tm_hour = 6;
    ptm->tm_min = 0;
    ptm->tm_sec = 0;
    int today6 = mktime(ptm);
    if (today6 > curSec)
        today6 -= 0x15180;
    int yesterday6 = today6 - 0x15180;
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_10,
                                                  E_SERVER_GROUP_NONE);
    unsigned char* buf =
        (unsigned char*)stream->GetOutBuffer<Packet_PvPResult>();
    int outer = 0;
    while (outer < ((DB_UpdatePvPResultHeadView*)buf)->m_count)
    {
        db->set_query(
            "seLect school_id, rating, fight_cnt, unix_timestamp(last_update_time), school_point, school_point_prev, fight_cnt_prev from school_info where school_id in (%d,%d)",
            ((DB_UpdatePvPResultSlotView*)(buf + 0 * 0x38))->m_schoolIds[outer],
            ((DB_UpdatePvPResultSlotView*)(buf + 1 * 0x38))->m_schoolIds[outer]);
        if (!db->exec(true))
            return 0;
        int needRows = db->get_n_rows();
        if (needRows != 2)
            return 0;
        int nRow = 0;
        while (nRow < needRows)
        {
            // 4.1 循环布局：fetch 在循环体顶部（`call; xor$1; test; jne 出口`），
            // 条件 nRow<needRows 在底部；`A && B()` 合写在 4.4 会改为底部 fetch。
            if (!db->fetch())
                break;
            if (!db->get_int(0, row.m_schoolId))
                return 0;
            if (!db->get_float(1, row.m_rating))
                return 0;
            if (!db->get_int(2, row.m_fightCnt))
                return 0;
            if (!db->get_int(3, row.m_lastUpdateTime))
                return 0;
            if (!db->get_int(4, row.m_schoolPoint))
                return 0;
            if (!db->get_int(5, row.m_schoolPointPrev))
                return 0;
            if (!db->get_int(6, row.m_fightCntPrev))
                return 0;
            row.m_ratingPrev = row.m_rating;
            if (((DB_UpdatePvPResultSlotView*)(buf + 0 * 0x38))->m_schoolIds[outer] ==
                row.m_schoolId)
                rows[0] = row;
            else if (((DB_UpdatePvPResultSlotView*)(buf + 1 * 0x38))->m_schoolIds[outer] ==
                     row.m_schoolId)
                rows[1] = row;
            else
                return 0;
            ++nRow;
        }
        CEloRating::calc(rows[0].m_rating, rows[1].m_rating,
                         (ENUM_ELO_RATING_SYSTEM)((DB_UpdatePvPResultHeadView*)buf)->m_eloSys,
                         ((DB_UpdatePvPResultHeadView*)buf)->m_eloK);
        for (int i = 0; i <= 1; ++i)
        {
            rows[i].m_fightCntCur =
                (int)(rows[i].m_rating - rows[i].m_ratingPrev);
            if (rows[i].m_lastUpdateTime - today6 <= 0)
            {
                if (rows[i].m_lastUpdateTime - yesterday6 > 0)
                {
                    rows[i].m_fightCntPrev = rows[i].m_fightCntCur;
                    rows[i].m_schoolPointPrev = rows[i].m_schoolPoint;
                    rows[i].m_fightCntCur = 0;
                    rows[i].m_schoolPoint = 0;
                }
                else
                {
                    rows[i].m_fightCntPrev = 0;
                    rows[i].m_schoolPointPrev = 0;
                    rows[i].m_fightCntCur = 0;
                    rows[i].m_schoolPoint = 0;
                }
            }
            ++rows[i].m_fightCntCur;
            if (rows[i].m_fightCntCur ==
                PvPGlobalEnvironmentVariable::school_match_fight_count_for_gain_point_)
                rows[i].m_schoolPoint =
                    PvPGlobalEnvironmentVariable::school_match_daily_point_;
            else if (rows[i].m_fightCntCur == 15)
            {
                if (wday == 0 || wday == 6)
                    rows[i].m_schoolPoint =
                        PvPGlobalEnvironmentVariable::school_match_daily_point_weekends_;
                if (curSec > 0x48f26550 && curSec <= 0x48f8fccf)
                    rows[i].m_schoolPoint =
                        PvPGlobalEnvironmentVariable::school_match_daily_point_weekends_;
            }
            db->set_query(
                "upDate school_info set fight_cnt=%d,school_point=%d, fight_cnt_prev=%d, school_point_prev=%d, rating=%f, last_update_time=from_unixtime(%d) where school_id=%d",
                rows[i].m_fightCntCur,
                rows[i].m_schoolPoint,
                rows[i].m_fightCntPrev,
                rows[i].m_schoolPointPrev,
                (double)rows[i].m_rating,
                curSec,
                ((DB_UpdatePvPResultSlotView*)(buf + i * 0x38))->m_schoolIds[outer]);
            if (!db->exec(true))
                return 0;
            // P227：全表达式临时链 ⇒ 4.4 在 ctor 前装载 op() 实参（ORIG 69 条
            // 装载块位于 ctor 之前；具名对象时 4.4 改到 ctor 之后，缺口 -59）。
            cMyTrace(__PRETTY_FUNCTION__, 0x7867, 5)(
                "fight_cnt=%d,school_point=%d, fight_cnt_prev=%d, school_point_prev=%d, rating=%f, last_update_time=%d, school_id=%d",
                rows[i].m_fightCntCur,
                rows[i].m_schoolPoint,
                rows[i].m_fightCntPrev,
                rows[i].m_schoolPointPrev,
                (double)rows[i].m_rating,
                curSec,
                ((DB_UpdatePvPResultSlotView*)(buf + i * 0x38))->m_schoolIds[outer]);
        }
        ++outer;
    }
    int r = 0;
    while (r < ((DB_UpdatePvPResultHeadView*)buf)->m_count)
    {
        int p;
        for (p = 0; p <= 1; ++p)
        {
            // P183 地址重物化铁律：ORIG 每处 badge/dailyBadge 字段访问都从
            // buf + 两下标（r×3 / p×0x38）重算完整地址链（11 条/处）；缓存
            // 具名 slotv 指针每处省 5 条 ⇒ 缺口 -54。全表达式展开。
            if (((DB_UpdatePvPResultSlotView*)(buf + p * 0x38))->m_badges[r].m_red != 0 ||
                ((DB_UpdatePvPResultSlotView*)(buf + p * 0x38))->m_badges[r].m_blue != 0)
                ++((DB_UpdatePvPResultSlotView*)(buf + p * 0x38))->m_badges[r].m_green;
            // P227：同上（ORIG NumberToString 在 ctor 之前 ⇒ 临时链形）。
            cMyTrace(__PRETTY_FUNCTION__, 0x7881, 0)(
                "BADGE_COUNT m_id(%s) badge green(%d), red(%d), blue(%d), daily badge green(%d), red(%d), blue(%d)",
                NumberToString(((DB_UpdatePvPResultSlotView*)(buf + p * 0x38))->m_ids[r], 0),
                ((DB_UpdatePvPResultSlotView*)(buf + p * 0x38))->m_badges[r].m_green,
                ((DB_UpdatePvPResultSlotView*)(buf + p * 0x38))->m_badges[r].m_red,
                ((DB_UpdatePvPResultSlotView*)(buf + p * 0x38))->m_badges[r].m_blue,
                ((DB_UpdatePvPResultSlotView*)(buf + p * 0x38))->m_dailyBadges[r].m_green,
                ((DB_UpdatePvPResultSlotView*)(buf + p * 0x38))->m_dailyBadges[r].m_red,
                ((DB_UpdatePvPResultSlotView*)(buf + p * 0x38))->m_dailyBadges[r].m_blue);
            db->set_query(
                "upDate school_member set contribute=contribute+%d, last_play_time=NOW(), green_badge=green_badge+%d, red_badge=red_badge+%d, blue_badge=blue_badge+%d, daily_green_badge=%d, daily_red_badge=%d, daily_blue_badge=%d  where m_id=%s and school_id=%d",
                rows[p].m_fightCntCur,
                ((DB_UpdatePvPResultSlotView*)(buf + p * 0x38))->m_badges[r].m_green,
                ((DB_UpdatePvPResultSlotView*)(buf + p * 0x38))->m_badges[r].m_red,
                ((DB_UpdatePvPResultSlotView*)(buf + p * 0x38))->m_badges[r].m_blue,
                ((DB_UpdatePvPResultSlotView*)(buf + p * 0x38))->m_dailyBadges[r].m_green,
                ((DB_UpdatePvPResultSlotView*)(buf + p * 0x38))->m_dailyBadges[r].m_red,
                ((DB_UpdatePvPResultSlotView*)(buf + p * 0x38))->m_dailyBadges[r].m_blue,
                NumberToString(((DB_UpdatePvPResultSlotView*)(buf + p * 0x38))->m_ids[r], 0),
                ((DB_UpdatePvPResultSlotView*)(buf + p * 0x38))->m_schoolIds[r]);
            if (!db->exec(true))
                return 0;
        }
        ++r;
    }
    return 1;
