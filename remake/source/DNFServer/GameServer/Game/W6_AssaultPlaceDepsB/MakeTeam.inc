void CAssaultPlace::MakeTeam(CUser* user1, CUser* user2, int& outIdx)
{
    // param_2 存 +0x134、param_3 存 +0x138——与既有头字段一一对应。
    m_pos.m_x = user1->getCurCharacVill();
    m_pos.m_y = user1->get_area(false);
    if (m_pos.m_x == 0 && m_pos.m_y == 0)
        throw (ENUM_ERROR)0x15;
    m_checkUser = user1;                                        // +0x134
    m_field138 = user2;                                         // +0x138

    // posX 比较：ORIG 首 get_posX 结果 `mov %eax,%ebx` 跨第二 call 存寄存器
    // （P197② 语句表达式）；随后 _SeparateTeam 两参顺序随比较翻转。
    // [b28d-solo-W6_AssaultPlaceDepsB] 真 bug 修正：ORIG then（posX1<posX2，
    //   fall-through）为 `_SeparateTeam(user1,user2)` + m_winTeam=0，else 为
    //   `_SeparateTeam(user2,user1)` + m_winTeam=1；原写 then/else 实参序颠倒。
    // [b28d-solo-W6_AssaultPlaceDepsB] P226：裸语句表达式（内无具名变量）才令
    //   值升格 callee-saved；带具名 r 会槽化（多 mov+movzwl 两条）。
    if (({ user1->get_posX(); }) < user2->get_posX()) {
        _SeparateTeam(user1, user2);
        m_winTeam = (ENUM_ASSAULT_TEAM)0;                       // +0x124
    } else {
        _SeparateTeam(user2, user1);
        m_winTeam = (ENUM_ASSAULT_TEAM)1;                       // +0x124
    }

    m_authenValue = GlobalData::s_systemTime_.getCurSec();       // +0x108 = getCurSec
    _SetReadyState();

    if (m_parties[0].m_field00 != 0)                            // +0x12c
        m_parties[0].m_field00->SetAssaultState('\x02');
    if (m_parties[1].m_field00 != 0)                            // +0x130
        m_parties[1].m_field00->SetAssaultState('\x02');

    // 8 个名字槽（.rodata 空串 DAT_08c1bca0 直存），team0 填 [0..]，team1 填 [4..]。
    // [b28d-solo-W6_AssaultPlaceDepsB] P212：ORIG 槽深序 names(-0x4c..-0x30) >
    //   i(-0x2c) > team1Count(-0x28) > team0Count(-0x24) ⇒ i 提至函数头并声明在
    //   team 计数之前（两个循环共用同一 i）。
    const char* names[8] = { "", "", "", "", "", "", "", "" };
    int i;
    int team0Count = 0;                          // ORIG -0x28
    int team1Count = 0;                          // ORIG -0x24
    for (i = 0; i <= 7; ++i) {
        if (m_assaulters[i].Empty())
            continue;
        if (m_assaulters[i].GetTeam() == 0) {
            names[team0Count] = m_assaulters[i].GetCharacName();
            team0Count++;
        } else if (m_assaulters[i].GetTeam() == 1) {
            names[team1Count + 4] = m_assaulters[i].GetCharacName();
            team1Count++;
        }
    }
    for (i = 0; i <= 7; ++i) {
        CUser* u = m_assaulters[i].GetUser();
        if (u != 0) {
            int reason;                                          // ORIG sete/test 形
            if (u->getCurCharacVill() == 7)
                reason = 0;
            else
                reason = 4;
            u->m_historyLog.StartPvP((ENUM_PVP_HISTORY_REASON)reason,
                                     names[0], names[1], names[2], names[3],
                                     names[4], names[5], names[6], names[7]);
        }
    }
