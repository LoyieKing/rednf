bool DB_UpdatePvPGrade::dispatch(int userIdx, int slot, Stream* stream)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2,
                                                  E_SERVER_GROUP_NONE);
    db->set_query(
        "seLect charac_no,pvp_grade,pvp_point,unix_timestamp(last_play_time) from pvp_grade_expand where pvp_grade>=%d order by pvp_point desc limit 1500",
        GlobalData::s_ref_pvp_grade->getPvPRankRateGrade());
    // P84：短路 bool 物化（ORIG setne/xor/test 合并守卫）。
    if (!(db->exec(true) && db->get_n_rows() != 0))
        return 0;

    time_t cur = GlobalData::s_systemTime_.getCurSec();
    struct tm tmv;
    localtime_r(&cur, &tmv);
    tmv.tm_hour = 1;
    tmv.tm_min = 0;
    tmv.tm_sec = 0;
    cur = mktime(&tmv);
    int limitTime = (int)cur - 0x15180;

    std::vector<stPvPTempTable_t> rows;
    // ORIG 以循环构造相邻两 vector（edx/edi 指针步 0xc）：源码形态为数组。
    std::vector<int> lists[2];    // [0] 扣减列表 @ -0x90 / [1] 置回列表 @ -0x84
    int refPoint = GlobalData::s_ref_pvp_grade->GetPvpPoint(
        GlobalData::s_ref_pvp_grade->getPvPExpLossGrade());
    // SQL 行字段（ORIG 声明序先于 rate 数组，直传槽给 get_uint/get_int）。
    unsigned int characNo;
    int pvpGrade;
    int pvpPoint;
    unsigned int lastPlay;
    unsigned int i = 0;
    while (i < (unsigned int)db->get_n_rows())
    {
        if (db->fetch() != 1)
            return 0;
        if (!db->get_uint(0, characNo))
            return 0;
        if (!db->get_int(1, pvpGrade))
            return 0;
        if (!db->get_int(2, pvpPoint))
            return 0;
        if (!db->get_uint(3, lastPlay))
            return 0;
        if (GlobalData::s_ref_pvp_grade->getPvPExpLossGrade() <= pvpGrade &&
            (int)lastPlay <= limitTime && refPoint < pvpPoint)
        {
            if (GlobalData::s_ref_pvp_grade->getPvPExpLossExp() + refPoint <
                pvpPoint)
                // [b28d-solo] P270：ORIG 调 push_back(int&&)（EOi），
                // 值传选中 const& 版本 ⇒ 显式右值 cast。
                lists[0].push_back(static_cast<int&&>((int)characNo));
            else
                lists[1].push_back(static_cast<int&&>((int)characNo));
            int tmp = pvpPoint -
                      GlobalData::s_ref_pvp_grade->getPvPExpLossExp();
            pvpPoint = std::max(refPoint, tmp);
        }
        // ORIG push_back 直传 charac_no 槽（元素首 4 字节即 m_characNo）。
        rows.push_back(*(stPvPTempTable_t*)&characNo);
        ++i;
    }

    int rowCount = (int)rows.size();   // -0x48
    // [b28d-solo] P167：ORIG 用 `operator[](unsigned) const` ⇒ 绑定 const 引用。
    const std::vector<int>& rankRate =
        GlobalData::s_ref_pvp_grade->getPvPRankRate();   // -0x44
    int rankCount = 4;                 // -0x40
    int rate[5] = {0, 0, 0, 0, 0};     // -0xa8..-0x98
    for (int r = 0; r < 5; ++r)
        rate[r] = (rankRate[r] * rowCount + 500) / 1000;
    for (int r = 1; r < 5; ++r)
    {
        if (rate[r - 1] > 0)
            rate[r] = std::max(rate[r] - rate[r - 1], 1);
    }
    if (rowCount != 0 && rate[3] == 0)
        rate[4] = 1;

    char buf[0x6000];
    memset(buf, 0, 0x6000);
    if (!lists[0].empty())
    {
        db->set_query(
            "upDate pvp_result set pvp_point=pvp_point-%d where charac_no %s",
            GlobalData::s_ref_pvp_grade->getPvPExpLossExp(),
            getList2inQuery(lists[0], buf));
        if (db->exec(true) != 1)
            return 0;
    }
    if (!lists[1].empty())
    {
        db->set_query(
            "upDate pvp_result set pvp_point=%d where charac_no %s",
            refPoint, getList2inQuery(lists[1], buf));
        if (db->exec(true) != 1)
            return 0;
    }

    std::vector<int> gradeList[5];     // -0x120 起（5×vector<int>）
    int gradeIdx = 0;                  // -0x3c
    int gradeLimit = rate[0];          // -0x38
    unsigned int k = 0;                // -0x24
    while (k < (unsigned int)rows.size())
    {
        if (gradeLimit == 0)
        {
            char done = 0;             // -0x1d
            do
            {
                ++gradeIdx;
                if (gradeIdx == 5)
                {
                    done = 1;
                    break;
                }
                gradeLimit = rate[gradeIdx];
            } while (rate[gradeIdx] == 0);
            if (done != 0)
                break;
        }
        // [b28d-solo] P270：ORIG push_back(int&&)（p167 -1 EOi/+1 ERKi）。
        gradeList[gradeIdx].push_back(
            static_cast<int&&>(rows[k].m_characNo));
        --gradeLimit;
        ++k;
    }

    time_t now = GlobalData::s_systemTime_.getCurSec();   // -0x34
    for (int g = 0; g < 5; ++g)
    {
        if (!gradeList[g].empty())
        {
            db->set_query(
                "upDate pvp_result set pvp_grade_ext=%d, pvp_grade_ext_update_time=from_unixtime(%d) where charac_no %s",
                5 - g, (int)now, getList2inQuery(gradeList[g], buf));
            if (db->exec(true) != 1)
                return 0;
        }
    }
    return 1;
