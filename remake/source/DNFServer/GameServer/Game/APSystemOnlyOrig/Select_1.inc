bool DB_LoadRewardMedal::Select(_SIG_LOAD_REWARD_MEDAL& sig)
{
    // 伪 helper 归一：ORIG 直接调 DBMgr::GetDBHandle
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2, E_SERVER_GROUP_NONE);
    int dailyHour =
        G_CEnvironment()->m_serverEnvir.m_fatigueTime;   // +0x37c
    time_t dailyTime = getDailyInitializeTime(dailyHour, 0, 0);
    time_t yesterdaySec = dailyTime - 0x93a80;

    struct tm tmToday;
    struct tm tmYesterday;
    // P0：Ghidra C 无 today/prev 中间局部 —— localtime_r 直接取 dailyTime 槽地址
    localtime_r(&dailyTime, &tmToday);
    localtime_r(&yesterdaySec, &tmYesterday);
    // [b28d-solo-APSystemOnlyOrig] ORIG 0x08124910：两个 sprintf 目标各 16 字节
    // （Ghidra local_40[16]/local_50[16]；旧版 11 字节短 5）。
    char strToday[16];
    char strYesterday[16];
    sprintf(strToday, "%04d-%02d-%02d", tmToday.tm_year + 1900, tmToday.tm_mon + 1,
            tmToday.tm_mday);
    sprintf(strYesterday, "%04d-%02d-%02d", tmYesterday.tm_year + 1900,
            tmYesterday.tm_mon + 1, tmYesterday.tm_mday);

    const std::vector<_MedalRewardItem>& medals =
        G_CDataManager()->GetActionPointEtcParameter(CActionPointManager::_IsGMOn != 0)->m_medalRewards;

    // [b28d-solo-APSystemOnlyOrig] ORIG 0x08124910：size() 先于 99999 初始化
    // （Ghidra local_1c=size、local_18=99999）。
    std::size_t medalCount = medals.size();
    // [round5-C05 2026-09-10] S1 一次性旋钮：ORIG 0x08124a5a `cmp -0x14(%ebp),%eax;
    // setl` 为**内存直比**（无转换中转）⇒ RHS 是普通 int 局部（T1–T4 对 minAp
    // 无直证：仅赋值/比较/实参）。unsigned 形 4.4 多一条 `mov -0x14(%ebp),%eax`
    // 并把字段值挪到 %edx（167 vs 166 唯一残差）。
    int minAp = 99999;
    for (unsigned int i = 0; i < medalCount; ++i)
    {
        if (static_cast<int>(medals[i].m_field00) < minAp)
        {
            minAp = medals[i].m_field00;
        }
    }

    db->set_query("select occ_date, ap_sum  \t\t\t\t\tfrom charac_action_point\t\t\t\t\t\twhere charac_no = %u\t\t\t\t\t\tand is_reward_medal = %u \t\t\t\t\tand occ_date < '%s'\t\t\t\t\t\tand occ_date >= '%s'\t\t\t\t\t\tand ap_sum >= %d",
                  sig.m_characNo, 0, strToday, strYesterday,
                  static_cast<int>(minAp));
    if (!db->exec(true))
    {
        return false;
    }
    // P0：Ghidra C 具名局部 local_14 = get_n_rows()
    int nRows = db->get_n_rows();
    // [round5-C05 2026-09-10] ORIG 0x08124aff `cmpl $0x0,-0x10; jle 08124b7f`
    // ⇒ nRows 失败出口 = 函数尾部独立块（+0x26f `mov $0,%eax` 落 leave），
    // 同 DB_LoadActionPoint::Select（探针 /tmp/tail_probe.cpp fA/fB）。
    if (nRows >= 1)
    {
        if (!db->fetch())
        {
            return false;
        }
        if (!db->get_str(0, sig.m_occDate, 0xb))
        {
            return false;
        }
        // [b28d-solo-APSystemOnlyOrig] ORIG 0x08124b4d：`mov 0xc(%ebp),%eax;
        // add $0x4,%eax` 直传 &sig.m_apSum（packed 字段经 char* 基址转写）。
        if (!db->get_uint(1, *reinterpret_cast<unsigned int*>(
                                 reinterpret_cast<char*>(&sig) + 4)))
        {
            return false;
        }
        return true;
    }
    return false;
