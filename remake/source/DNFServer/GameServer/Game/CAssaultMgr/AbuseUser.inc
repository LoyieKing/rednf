void CAssaultPlace::AbuseUser(CUser* user)      // ORIG T 0x82eb5e2（size 0x192）
{
    // b28d-solo-CAssaultMgr: IDENTICAL_AE（清单 NEAR 1 → AE；446h46 路由口径）
    // b28d-L3: IDENTICAL_AE — b28d-L3：446h46 路由后 fresh 复验 AE（TSV 的 NEAR 系 447 口径 stale 伪差
    // ORIG 0x82eb5e2 转写：检查段第一次 getCurSec（`movl $0x941f714,(%esp)`），
    // 块内**第二次独立** getCurSec 重算差值入 callee-saved %esi（P48 双臂
    // 双 call），getCurCharacNo 入 %ebx；cMyTrace 用**临时链式**（P37：
    // varargs 表达式分到 callee-saved，命名对象则提前落槽多装载）。
    // 比较 `cmp $0x13,%eax; setle %al; test; je`（P51 setcc 物化）。
    if ((int)(GlobalData::s_systemTime_.getCurSec() -
              m_abuseSec) <= 19)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0xc23, 0)(
            "CAssaultPlace::AbuseUser() charac_no(%d), timeout sec(%d)",
            user->getCurCharacNo(),
            (int)(GlobalData::s_systemTime_.getCurSec() - m_abuseSec));
    }
    if (m_state == 4)
    {
        for (int j = 0; j <= 7; ++j)
        {
            if (m_assaulters[j].Empty())
                continue;
            // ORIG 82eb703：两次 GetUser 均无槽往返（P0 ⇒ 原码无具名局部，
            // 值右值直传）：第一次直落 getHackAnalyzer receiver 链，第二次
            // 结果经 %ebx 中转作 arg1（4.1 冗余 regmove=N18a 已证不可造，
            // 但 b28 实测**实参位内联** 4.4 自然分配 %ebx 逐条全等；
            // register asm("ebx") 绑定违例已按 P0/N96 清除）。共 2 次 GetUser。
            m_assaulters[j].GetUser()
                ->getHackAnalyzer()
                ->addServerHackCnt(m_assaulters[j].GetUser(),
                                   (WongWork::ENUM_HACKTYPE)0x32e, 1, 0, 0);
        }
        m_state = (ENUM_ASSAULT_STATE)6;
        // [SweepCAssaultMgr 真语义修正] ORIG 082eb672 `cmp $0x4; jne 082eb76d`
        // （目标 = epilogue）与 Ghidra C（AbuseUser.md:154-171）同证：party
        // SetAssaultState(6)×2 与 TimeoutCloseAssault **在 `if (m_state == 4)`
        // 块内**。旧形置于块外 ⇒ m_state != 4 时仍改 party 状态并强制关场
        // （真语义反向；AE 口径把分支目标归一为 <T> 故不显形）。
        if (m_parties[0].m_field00 != 0)
            ((CParty*)m_parties[0].m_field00)->SetAssaultState(6);
        if (m_parties[1].m_field00 != 0)
            ((CParty*)m_parties[1].m_field00)->SetAssaultState(6);
        TimeoutCloseAssault();
    }
