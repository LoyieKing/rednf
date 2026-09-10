bool WongWork::CSimpleSSO::updateSSOExpireTime(unsigned int mId,
                                               const stUpdateSSOExpireTime_t& data)
{
    // [b28d-solo-WongWorkSSO_A] ORIG 0x08602906：89=89 指令、p167 8/8 CLEAR。
    // 已闭合：reportCnt 无槽（P0 内联实参）；idStr 走裸 register（P231 合规，
    // ORIG %ebx 无槽）；rating 为 double 局部（ORIG flds 0xc+fstpl -0x48）；
    // tutorialSkipable=(unsigned char) 转换（ORIG movzbl 双段）；两次 getCurSec
    // 内联实参（%esi/%ecx）；字段局部「先声明后赋值」两句式（P198①）。
    // N 封顶（NEAR，残差=槽号镜像+callee-saved 角色）：
    // ① 4.4-O0 恒将局部数组置于槽区最深处（探针 mA/mB/mC/mH：tableName 恒 -0x40，
    //    加 0x20 dead int 后 -0x60），4.1 按声明序（ORIG tableName -0x2c 在标量之上、
    //    标量 -0x4c..-0x3c 在数组之下）；标量/数组相对深度互为镜像，合法源形不可翻
    //    （N82/P112）。实测本件 tableName -0x40 vs -0x2c、标量 -0x2c..-0x1c vs
    //    -0x4c..-0x3c，帧 0x6c vs 0x7c（随槽基差）。
    // ② idStr/reportCnt/getCurSec#1 的 callee-saved 角色互换（ORIG ebx/edi/esi vs
    //    我方 edi/esi/ebx；N7/N82附）。语义/调用链/字段列/实参序全等。
    unsigned int tutorialSkipable;
    double rating;
    unsigned int dungeonGainGold;
    unsigned int tradeGoldDaily;
    char tableName[0x14];
    memset(tableName, 0, 0x14);                // ORIG call memset
    memcpy(tableName, "member_login", 0xd);    // ORIG call memcpy
    register char* idStr = NumberToString(mId, 0);

    // ORIG movzbl 0x10(%eax),%eax; movzbl %al,%eax; mov %eax,-0x4c(%ebp)
    // ⇒ int 槽 + (unsigned char) 转换（P266/P267 同族）。
    tutorialSkipable = (unsigned char)data.m_tutorialSkipable;  // +0x10
    rating = data.m_rating;                          // +0x0c（double 槽 fstpl）
    dungeonGainGold = data.m_dungeonGainGold;        // +0x08
    tradeGoldDaily = data.m_tradeGoldDaily;          // +0x04

    m_mysql->set_query(
        "upDate %s set expire_time=%d,last_play_time=%d,report_cnt=%d,"
        "trade_gold_daily=%d,dungeon_gain_gold=%d,rating=%f,tutorial_skipable=%d"
        " where m_id=%s",
        tableName, GlobalData::s_systemTime_.getCurSec() + 0xb4,
        GlobalData::s_systemTime_.getCurSec(), data.m_reportCnt, tradeGoldDaily,
        dungeonGainGold, rating, tutorialSkipable, idStr);
    if (!m_mysql->exec(true))
        return false;
    if (m_mysql->getAffectedRowCount() == 0)
        return false;
    return true;
