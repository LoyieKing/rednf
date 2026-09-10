// ============================================================================
// getBreakAwayMemberData（ORIG T 0x084101ae）
// ============================================================================

// [S414_DB_Login] verdict=DIFF ndiff=224 gap=-2（原 272/30）：P312——分支内
// `char playInfo[31] = {0}`（槽基 -0x47 非 4 对齐）复现 ORIG 0x0841026e 的 4.1
// 内联 memset 慢路（对齐剥离 + 主 4B 循环 + 尾 2/1，逐条全等）；旧 for 逐字节形
// = N78b 朴素 9 条形。余 = N82 帧（ORIG `push esi/ebx + sub 0x80` vs 4.4
// `sub 0x98`）致全帧槽号偏移；p167 CLEAR 44/44。
bool DB_Login::getBreakAwayMemberData(SIG_LOGIN_DATA* sig)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_6, E_SERVER_GROUP_NONE);
    db->set_query(
        "seLect accrue_cera, play_info, last_reward_time, first_reward_time, luck_point, last_update_time, add_info, second_reward_time, quest_time from churn_member_info where m_id=%s",
        NumberToString(sig->m_accId, 0));
    if (!db->exec(true))
    {
        LogManager::logFormat(1, "DBThread.cpp",
                              __PRETTY_FUNCTION__, 0x307b,
                              "BREAK_AWAY : churn_member_info DB ERROR!!");
        return 0;
    }

    if (db->get_n_rows() == 0)
    {
        // [solo] P312：ORIG 0x0841026e 的 4.1 内联 memset 慢路（对齐剥离 + 主 4B
        // 循环 + 尾 2/1）由 `char buf[31] = {0}` 且槽基非 4 对齐复现（ndiff 272→224，
        // gap 30→-2；旧 for 逐字节形 = N78b 朴素 9 条形）。
        char playInfo[31] = {0};             // ORIG local_5f[0x1f]（-0x5b）
        memset(playInfo, 0x30, 0x1e);        // '0'×30（初始 play_info 占位）
        playInfo[29] = '1';                  // ORIG movb $0x31,-0x3e = buf+0x1d
        db->set_query(
            "inSert into churn_member_info(m_id,accrue_cera,play_info,last_reward_time,first_reward_time,server_id,charac_no,item_id,add_info,luck_point,last_update_time,second_reward_time) values(%s,0,'%s',0,0,0,0,0,0,0,0,0)",
            NumberToString(sig->m_accId, 0), playInfo);
        if (!db->exec(true))
        {
            LogManager::logFormat(1, "DBThread.cpp",
                                  __PRETTY_FUNCTION__, 0x308c,
                                  "BREAK_AWAY : churn_member_info DB INSERT FAIL!!");
            return 0;
        }
        cMyTrace trace(__PRETTY_FUNCTION__, 0x308f, 0);
        trace("BREAK_AWAY : churn_member_info MEMBER INSERT!! M_ID(%s)",
              NumberToString(sig->m_accId, 0));

        // STBreakAwayMemberData::clear（ORIG W 0x844d584；rec = +0x3f18 起 0x41 字节）
        reinterpret_cast<STBreakAwayMemberData*>(&sig->m_breakAwayAccureCera)->clear();
        memcpy(sig->m_breakAwayPlayInfo, playInfo, 0x1f);
        return 1;
    }

    if (!db->fetch())
    {
        cMyTrace trace(__PRETTY_FUNCTION__, 0x309a, 5);
        trace("BREAK_AWAY : churn_member_info NO MEMBER USER!! M_ID(%s)",
              NumberToString(sig->m_accId, 0));
        return 0;
    }

    if (!db->get_int(0, sig->m_breakAwayAccureCera)) return 0;                   // accrue_cera
    if (!db->get_str(1, reinterpret_cast<char*>(sig->m_breakAwayPlayInfo), 0x1f)) return 0;  // play_info
    if (sig->m_breakAwayPlayInfo[0] == '\0')
    {
        LogManager::logFormat(1, "DBThread.cpp",
                              __PRETTY_FUNCTION__, 0x30ab,
                              "BREAK_AWAY : ABNOMAL PLAY_INFO DATA!! M_ID(%s)",
                              NumberToString(sig->m_accId, 0));
        return 0;
    }
    if (!db->get_uint(2, sig->m_breakAwayLastRewardTime)) return 0;   // last_reward_time(+0x3f3c)
    if (!db->get_uint(3, sig->m_breakAwayRewardBoundA)) return 0;   // first_reward_time(+0x3f40)
    if (!db->get_int(4, sig->m_breakAwayLuckPoint)) return 0;             // luck_point(+0x3f54)
    if (!db->get_uint(5, sig->m_breakAwayLastUpdateTime)) return 0;   // last_update_time(+0x3f50)
    if (!db->get_int(6, sig->m_breakAwayDungeonClearCnt)) return 0;             // add_info(+0x3f48)
    if (!db->get_uint(7, sig->m_breakAwayRewardBoundB)) return 0;   // second_reward_time(+0x3f44)
    if (!db->get_uint(8, sig->m_breakAwayQuestTime)) return 0;   // quest_time(+0x3f4c)

    // last_reward_time 跨日（days>0）→ luck_point 清零并回写
    if (sig->m_breakAwayLastRewardTime != 0)
    {
        long prev = getPrevScheduleTime(0, (long)sig->m_breakAwayLastRewardTime);
        int curSec = GlobalData::s_systemTime_.getCurSec();
        unsigned int days = (unsigned int)((curSec - prev) / 0x15180);
        unsigned int zero = 0;
        unsigned int maxDays = std::max<unsigned int>(zero, days);   // ORIG call _ZSt3maxIjERKT_S2_S2_
        if (maxDays != 0 && sig->m_breakAwayLuckPoint != 0)
        {
            sig->m_breakAwayLuckPoint = 0;                     // luck_point 清零
            db->set_query("upDate churn_member_info set luck_point=%d where m_id=%s",
                          sig->m_breakAwayLuckPoint,
                          NumberToString(sig->m_accId, 0));
            bool updOk = db->exec(true);
            if (!(updOk && db->getAffectedRowCount() != 0))
            {
                LogManager::logFormat(1, "DBThread.cpp",
                                      __PRETTY_FUNCTION__,
                                      0x30d1, "BREAK_AWAY : LUCK POINT UPDATE ERROR!!");
                return 0;
            }
        }
    }

    // quest_time!=0 且已过期且命中奖励时点 → add_info=-1 / quest_time=0 回写
    if (sig->m_breakAwayQuestTime != 0)
    {
        // [C05② 实测回退·ORIG 符号性直证] ORIG 该比较为 `mov 0x3f4c(%eax),%ebx;
        // cmp %eax,%ebx; jae` —— **jae=无符号** ⇒ 原码局部必为 unsigned（改 int 实测
        // 生 `jge` 有符号，语义对负值分叉，属真偏差）。4.4 对 unsigned 具名局部恒折
        // `cmp -0x10(%ebp),%eax` 内存直比 vs 4.1 寄存器直比 ⇒ 跨版本差（N18b/P209
        // 族）封顶，维持 unsigned。
        unsigned int questTime = sig->m_breakAwayQuestTime;
        unsigned int nowSec = GlobalData::s_systemTime_.getCurSec();
        if (questTime < nowSec &&
            (sig->m_breakAwayLastRewardTime == sig->m_breakAwayRewardBoundA ||
             sig->m_breakAwayLastRewardTime == sig->m_breakAwayRewardBoundB))
        {
            sig->m_breakAwayDungeonClearCnt = -1;                    // add_info = -1
            sig->m_breakAwayQuestTime = 0;            // quest_time = 0
            db->set_query("upDate churn_member_info set add_info=%d, quest_time=0 where m_id=%u",
                          sig->m_breakAwayDungeonClearCnt,
                          sig->m_accId);
            bool updOk = db->exec(true);
            if (!(updOk && db->getAffectedRowCount() != 0))
            {
                LogManager::logFormat(1, "DBThread.cpp",
                                      __PRETTY_FUNCTION__,
                                      0x30e6, "BREAK_AWAY : ADD INFO UPDATE ERROR!!");
                return 0;
            }
        }
    }
    return 1;
}
