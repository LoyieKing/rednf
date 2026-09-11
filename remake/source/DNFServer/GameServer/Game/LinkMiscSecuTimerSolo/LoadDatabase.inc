int CAutoMarketConditionsControl::LoadDatabase(
    SIG_AUTO_MARKET_CONDITIONS_CONTROL* msg)
{
    long long base = 0;
    if (msg->m_view.m_flag != 0)
    {
        base = msg->m_view.m_overGold;
    }
    else
    {
        base = (msg->m_view.m_rows[1].m_totalGold +
                msg->m_view.m_rows[1].m_auctionGold) -
               (msg->m_view.m_rows[0].m_totalGold +
                msg->m_view.m_rows[0].m_auctionGold) +
               msg->m_view.m_rows[0].m_overGold;
    }

    GetMarketConditionRate(
        static_cast<double>(msg->m_view.m_optimumGold),
        static_cast<double>(base), m_goldRate,
        msg->m_view.m_rows[0].m_goldPhase, m_field00, m_goldBaseRate);
    GetMarketConditionRate(
        static_cast<double>(msg->m_view.m_optimumGold),
        static_cast<double>(base), m_equipProb,
        msg->m_view.m_rows[0].m_itemPhase, m_field380, m_equipProbBase);
    GetMarketConditionRate(
        static_cast<double>(msg->m_view.m_optimumGold),
        static_cast<double>(base), m_durRate,
        msg->m_view.m_rows[0].m_durabilityPhase, m_field3e8, m_durCurrent);

    if (msg->m_view.m_rows[0].m_durabilityPhase != m_field3e8)
        SendNotiSetDurability();

    int durNow = m_field3e8;    // ORIG -0x64（ctor 前具名槽）
    int equipNow = m_field380;  // ORIG -0x60
    // ORIG ctor call 前把余下三个变参读入 callee-saved（1df edi=this[0]、
    // 1e4 ebx=msg+8、1e7 esi=msg+c）跨调用保活（P87/P37 形；寄存器归属
    // 差异记 N7）。
    register int field00Now = m_field00;
    // ORIG 1e4/1e7 为**单次基址装载**读入 callee-saved 对（`mov 0xc(%ebp),%eax;
    // mov 0x8(%eax),%ebx; mov 0xc(%eax),%esi`）。两个 32 位独立视图读各发一次
    // 基址装载（+1 条 = 残差①）；改 `register unsigned long long` + `>>32` 拆字
    // 实测更劣（252 vs ORIG 246）⇒ 回退。**[b28e 补测 P227 全表达式临时链
    // （`cMyTrace(...)(...)` 构造即调用）]：fresh 读数 248 条 / 帧仍 `sub $0xac`
    // / 四数 4/30/0/5 全不变 ⇒ 对「ctor 前求值 + 单次基址装载」无改善，回退**；
    // 该段（含槽差 -0x30 vs ORIG -0x28、帧 +0x10）判 N80/N82 封顶。
    register unsigned int optGoldLo = msg->m_view.m_optimumGoldWords[0];
    register unsigned int optGoldHi = msg->m_view.m_optimumGoldWords[1];
    cMyTrace trace(
        __PRETTY_FUNCTION__,
        0xae, 0);
    trace("LoadDatabase() %qd %u %d %d %d",
          base,
          optGoldLo,
          optGoldHi,
          field00Now, equipNow, durNow);

    long long diff = base - msg->m_view.m_optimumGold;
    if (diff < 0)
        diff = 0;

    if (G_CEnvironment()->get_channel_no() == 1)
    {
        DB_UpdateAutoMarketConditionsControl::makeRequest(
            msg->m_view.m_rows[1].m_totalGold,
            msg->m_view.m_rows[1].m_auctionGold,
            diff,
            msg->m_view.m_optimumGold,
            m_field00, m_field380, m_field3e8);
    }
    return 0;
}