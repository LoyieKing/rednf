bool WongWork::CSimpleSSO::_getGameStartTime(unsigned int mId, long& loginTime,
                                             long& expireTime, stOutputData_t& out)
{
    // [b28d-solo-WongWorkSSO_A] ORIG 0x08602da6 复原：
    // ① idStr 无槽 ⇒ P0 内联实参 NumberToString；② exec/fetch 合并短路
    // （ORIG 汇聚块 mov$1/mov$0+test %al，P133/P214①）；③ loginValue/expireValue
    // 无预清（ORIG 直接 lea 传引用，P198①反例）；④ 原重建多一条 col++ 属真语义
    // bug：跳过 report_cnt 与 last_play_time 之间不存在的列，其后 11 列全部错位。
    char tableName[0x14];
    memset(tableName, 0, 0x14);       // ORIG +0x006..+0x01c call memset
    memcpy(tableName, "member_login", 0xd);  // ORIG +0x021..+0x037 call memcpy
    m_mysql->set_query(
        "seLect login_time,expire_time,report_cnt,last_play_time,account_fail,"
        "total_account_fail,reliable_flag,trade_gold_daily,login_ip,security_flag,"
        "dungeon_gain_gold,school_id,rating,tutorial_skipable,event_charac_flag"
        " from %s where m_id=%s",
        tableName, NumberToString(mId, 0));
    if (!m_mysql->exec(true) || !m_mysql->fetch())
        return false;
    if (m_mysql->get_n_rows() == 0)   // ORIG fetch 后空结果检查
        return false;

    unsigned int loginValue;   // col0 login_time  ORIG -0x24(%ebp)
    unsigned int expireValue;  // col1 expire_time ORIG -0x28(%ebp)

    int col = 0;
    if (!m_mysql->get_uint(col++, loginValue))
        return false;
    if (!m_mysql->get_uint(col++, expireValue))
        return false;
    if (!m_mysql->get_int(col++, out.m_reportCnt))  // report_cnt +0x0c
        return false;
    if (!m_mysql->get_int(col++, out.m_lastPlayTime))                       // last_play_time +0x00
        return false;
    if (!m_mysql->get_int(col++, out.m_accountFail))  // account_fail +0x04
        return false;
    if (!m_mysql->get_int(col++, out.m_totalAccountFail))  // total_account_fail +0x08
        return false;
    if (!m_mysql->get_byte(col++, out.m_reliableFlag))  // reliable_flag
        return false;
    if (!m_mysql->get_uint(col++, out.m_tradeGoldDaily))  // trade_gold_daily
        return false;
    if (!m_mysql->get_str(col++, out.m_loginIp, 0x14))  // login_ip +0x18
        return false;
    if (!m_mysql->get_byte(col++, out.m_securityFlag))  // security_flag
        return false;
    if (!m_mysql->get_uint(col++, out.m_dungeonGainGold))  // dungeon_gain_gold
        return false;
    if (!m_mysql->get_int(col++, out.m_schoolId))  // school_id
        return false;
    if (!m_mysql->get_float(col++, out.m_rating))  // rating
        return false;
    if (!m_mysql->get_ubyte(col++, out.m_tutorialSkipable))  // tutorial_skipable
        return false;
    if (!m_mysql->get_ubyte(col++, out.m_eventCharacFlag))  // event_charac_flag
        return false;
    // ORIG 尾部：*param_2=t1; *param_3=t2; +0x40=t1; +0x44=t2
    loginTime = static_cast<long>(loginValue);
    expireTime = static_cast<long>(expireValue);
    out.m_loginTime = loginValue;
    out.m_expireTime = expireValue;
    return true;
