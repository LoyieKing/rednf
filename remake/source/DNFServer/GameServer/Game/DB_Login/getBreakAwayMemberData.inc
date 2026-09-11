// [S414_DB_Login] verdict=DIFF ndiff=224 gap=-2（原 272/30）：P312——分支内
// `char playInfo[31] = {0}`（槽基 -0x47 非 4 对齐）复现 ORIG 0x0841026e 的 4.1
// 内联 memset 慢路（对齐剥离 + 主 4B 循环 + 尾 2/1，逐条全等）；旧 for 逐字节形
// = N78b 朴素 9 条形。余 = N82 帧（ORIG `push esi/ebx + sub 0x80` vs 4.4
// `sub 0x98`）致全帧槽号偏移；p167 CLEAR 44/44。
bool DB_Login::getBreakAwayMemberData(SIG_LOGIN_DATA* sig)
{
    // [帧撑实验 b28d-h9] ORIG 栈消耗 0x88（push esi + push ebx + sub 0x80）vs 我方
    // 0x94（push ebx + sub 0x90）⇒ 全帧槽号比 ORIG **浅 8 字节**（我方 lea
    // -0x53/-0x34/-0x24/-0x1c vs ORIG -0x5b/-0x3c/-0x2c/-0x18）。按 P232
    // 「4.4 -O0 对未读写局部照占栈」补 8 字节未用局部，试将槽基对齐 ORIG。
    char framePad[8];
    (void)framePad;
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
        // [P227] 单表达式临时链：ORIG 0x08410389 `call NumberToString; mov %eax,%ebx;
        // ... cMyTraceC1(-0x3c) ...; mov %ebx,0x8(%esp)`（varargs 在 ctor 前右起求值并
        // 跨 ctor 保活 %ebx）；两语句形会把 C1 提到 NumberToString 求值之前。
        cMyTrace(__PRETTY_FUNCTION__, 0x308f, 0)(
            "BREAK_AWAY : churn_member_info MEMBER INSERT!! M_ID(%s)",
            NumberToString(sig->m_accId, 0));

        // STBreakAwayMemberData::clear（ORIG W 0x844d584；rec = +0x3f18 起 0x41 字节）
        reinterpret_cast<STBreakAwayMemberData*>(&sig->m_breakAwayAccureCera)->clear();
        memcpy(sig->m_breakAwayPlayInfo, playInfo, 0x1f);
        return 1;
    }

    if (!db->fetch())
    {
        // [P227] 同上：ORIG 0x0841042a `call NumberToString; mov %eax,%ebx;
        // cMyTraceC1(-0x2c); mov %ebx,0x8(%esp)`。
        cMyTrace(__PRETTY_FUNCTION__, 0x309a, 5)(
            "BREAK_AWAY : churn_member_info NO MEMBER USER!! M_ID(%s)",
            NumberToString(sig->m_accId, 0));
        return 0;
    }

    if (!db->get_int(0, sig->m_breakAwayAccureCera)) return 0;                   // accrue_cera
    if (!db->get_str(1, reinterpret_cast<char*>(sig->m_breakAwayPlayInfo), 0x1f)) return 0;  // play_info
    // [SGBC 实验 b28d-h9] ORIG 0x084104e0 为 `mov 0xc(%ebp),%eax; add $0x3f1c,%eax;
    // movzbl (%eax),%eax`（指针算术 + 解引用，3 条）；成员数组直访问 `m_...Info[0]`
    // 被 4.4 折成 disp 单读（2 条）⇒ 改用 SGBC(sig,0x3f1c)[0] 的指针形复现 add。
    if (SGBC(sig, 0x3f1c)[0] == '\0')
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
        // [P0 去槽·ORIG 直证] ORIG 0x084106c8 `mov %eax,%ecx; sub -0x10(%ebp),%ecx`
        // （getCurSec 返回值直入 %ecx、无具名槽）⇒ curSec 不是原码局部，内联回表达式。
        // [回滚记录 b28d-h9] 曾试把 days/zero 声明上提函数头（ORIG 槽在浅区
        // -0x1c/-0x18，我方在深区 -0x60/-0x64），实测 ndiff 8→20 劣化 ⇒ 回滚，
        // 维持块内声明（4.4 槽分配序不可控，N82/N98 族）。
        unsigned int days =
            (unsigned int)((GlobalData::s_systemTime_.getCurSec() - prev) / 0x15180);
        unsigned int zero = 0;
        unsigned int maxDays = std::max<unsigned int>(zero, days);   // ORIG call _ZSt3maxIjERKT_S2_S2_
        if (maxDays != 0 && sig->m_breakAwayLuckPoint != 0)
        {
            sig->m_breakAwayLuckPoint = 0;                     // luck_point 清零
            db->set_query("upDate churn_member_info set luck_point=%d where m_id=%s",
                          sig->m_breakAwayLuckPoint,
                          NumberToString(sig->m_accId, 0));
            // [P0 去槽·ORIG 直证] ORIG 0x0841089f 段 `call exec; xor $1; test; jne`
            // （无 `mov %al,-0x15` 槽往返）⇒ updOk 不是原码局部，短路条件内联。
            if (!(db->exec(true) && db->getAffectedRowCount() != 0))
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
        // [P0 去槽·ORIG 直证] ORIG 0x084107f4 `mov 0x3f4c(%eax),%ebx`（**成员直读入
        // callee-saved**，非槽读）+ 0x08410801 `call getCurSec` + 0x08410806
        // `cmp %eax,%ebx; jae`（无符号）⇒ 原码为成员表达式与返回值直比；
        // questTime/nowSec 两个具名局部均非原码（各多一槽并撑大帧）。
        if (sig->m_breakAwayQuestTime < GlobalData::s_systemTime_.getCurSec() &&
            (sig->m_breakAwayLastRewardTime == sig->m_breakAwayRewardBoundA ||
             sig->m_breakAwayLastRewardTime == sig->m_breakAwayRewardBoundB))
        {
            sig->m_breakAwayDungeonClearCnt = -1;                    // add_info = -1
            sig->m_breakAwayQuestTime = 0;            // quest_time = 0
            db->set_query("upDate churn_member_info set add_info=%d, quest_time=0 where m_id=%u",
                          sig->m_breakAwayDungeonClearCnt,
                          sig->m_accId);
            // [P0 去槽·ORIG 直证] 同 luck_point 段：ORIG 0x0841089f 段无 bool 槽。
            if (!(db->exec(true) && db->getAffectedRowCount() != 0))
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