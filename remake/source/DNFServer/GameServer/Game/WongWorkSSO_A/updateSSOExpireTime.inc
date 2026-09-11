// ---- updateSSOExpireTime(mId, data)  ORIG 0x08602906
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
    // [M1S5 2026-09-11 新钥匙回扫 ⇒ 未推翻] ①P339 变体实测：把 `char tableName[0x14]`
    //   声明提到 4 个标量之前 ⇒ **零变化**（帧 0x6c、tableName 仍 -0x40、标量仍
    //   -0x2c/-0x28/-0x20/-0x1c；ndiff 84 逐字不变）⇒ 证实 4.4 恒将局部数组置于
    //   槽区最深处（与历史探针 mA/mB/mC/mH 一致），分区不可翻（P339 失效域②）；
    //   ②§6.6.2 P216 不适用（本件无 EH 段）；③§6.5 串盲区复核：SQL 串
    //   `upDate %s set expire_time=%d,…where m_id=%s`（ORIG 0x086029e3 内联 /
    //   我方 .rodata+0x2a4）与 `member_login`（ORIG 0x8cdc264 / 我方 +0x2c）
    //   逐字节全等；④尾部判定段逐条同形：`or %edx,%eax; test %eax,%eax; sete %al;
    //   test %al,%al; je; mov $0x0,%eax; jmp; mov $0x1,%eax`（ORIG
    //   0x08602a2b..0x08602a3d）⇒ 89=89、NEAR 维持。
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
}