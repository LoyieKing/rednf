bool CLeagueMatch::CheckMatchable(IMatch* other)   // ORIG T 0x0855fa14
{
    // ORIG 汇编对 other 直接按对象前部偏移读其第一队 PlayerInfo 槽
    // （this+slot*0x1c+0x24/0x28/0x2c），依赖 IMatch 派生类共享布局。
    // ORIG 无引用缓存：每次成员访问都重算 base+i*0x1c（每处 7 条寻址指令），
    // 且胜点比较是 MAX 宏形态 —— getPVPResultR()->m_pvpGrade 被求值两次；
    // 差值判定用 abs()（单临时槽 sar/xor/sub 展开），不得手写三遍表达式。
    int maxWinSelf = 0;
    int maxWinOther = 0;
    // [b28d-solo-CLeagueMatch] P0/P191：ORIG 0x0855fa1b/fa22/fa29/fa30 四清零
    // = maxWinSelf/maxWinOther + i 双初始化（`int i = 0;` + `for (i = 0;…)`）；
    // 三个循环共用同一 i 槽 -0x14（ORIG 0x0855faf8/fc1d 复用），非各自 for
    // 内声明（4.4 会各占独立槽，实测多 -0x20/-0x18 两槽）。
    int i = 0;

    for (i = 0; i < 4; ++i) {
        if (m_teamA.m_players[i].m_flag != 0 && m_teamA.m_players[i].m_user != 0)
            maxWinSelf =
                (m_teamA.m_players[i].m_user->getPVPResultR()->m_pvpGrade < maxWinSelf)
                    ? maxWinSelf
                    : m_teamA.m_players[i].m_user->getPVPResultR()->m_pvpGrade;
    }
    for (i = 0; i < 4; ++i) {
        if (reinterpret_cast<CLeagueMatch*>(other)->m_teamA.m_players[i].m_flag != 0 &&
            reinterpret_cast<CLeagueMatch*>(other)->m_teamA.m_players[i].m_user != 0)
            maxWinOther = (reinterpret_cast<CLeagueMatch*>(other)
                               ->m_teamA.m_players[i].m_user->getPVPResultR()->m_pvpGrade <
                           maxWinOther)
                              ? maxWinOther
                              : reinterpret_cast<CLeagueMatch*>(other)
                                    ->m_teamA.m_players[i]
                                    .m_user->getPVPResultR()
                                    ->m_pvpGrade;
    }

    // [b28d-solo-CLeagueMatch] 真语义 bug 修正（Ghidra C 对照 0x0855fbc0..fbf3）：
    // ORIG 第一组 `cmpl $0xb,self; jle 第二组 / cmpl $0xe,self; jg 第二组 /
    // cmpl $0xa,other; jg 第二组` ⇒ **跳过** return 0 的条件是 other > 10，
    // 即 return false 当 `self∈[12,14] && other<=10`（Ghidra `(10 < local_1c)`
    // 为跳转条件）；原实现写成 `other > 10` 恰把边界取反（真语义 bug）。
    if ((maxWinSelf >= 12 && maxWinSelf <= 14) && maxWinOther <= 10)
        return false;
    if ((maxWinOther >= 12 && maxWinOther <= 14) && maxWinSelf <= 10)
        return false;
    // [b28d-solo-CLeagueMatch] P0：ORIG 0x0855fc07 把 abs 结果写具名槽 -0x10
    // （mov %edx,-0x10(%ebp); sub %eax,-0x10(%ebp)）⇒ 具名 diff 局部。
    int diff = abs(maxWinSelf - maxWinOther);
    if (diff > 4)
        return false;

    for (i = 0; i < 4; ++i) {
        for (int b = 0; b < 4; ++b) {
            if (m_teamA.m_players[i].m_flag != 0 &&
                reinterpret_cast<CLeagueMatch*>(other)->m_teamA.m_players[b].m_flag != 0 &&
                m_teamA.m_players[i].m_schoolNo ==
                    reinterpret_cast<CLeagueMatch*>(other)->m_teamA.m_players[b].m_schoolNo)
                return false;
        }
    }
    return true;
