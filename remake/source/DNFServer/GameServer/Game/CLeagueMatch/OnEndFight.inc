// ORIG T 0x0855e55a
// [b28d-solo-CLeagueMatch] DIFF 45/45·ndiff 2·N-P75（S217 新鲜 .o 复验）：
// ORIG 0x0855e594 第二实参先入 %eax 再压常量 0，4.4 恒先压常量；cast/NULL/
// 具名局部 3 形不可翻（S180 追加逗号求值序形 ((void)managerTeam,(CUser*)0)
// 亦不变）。S217 再补 2 形：`0L` 与 `0` 同形；`*&managerTeam` 虽消实参序差但
// 把 `cmpl $1,-0x1c(%ebp)` 变 `mov;cmp $1,%eax`（46 条 +1，净零）⇒ 回滚。
// 共 6 形全败，语义等价。
void CLeagueMatch::OnEndFight(PvP_Room* room)
{
    unsigned int managerTeam = room->get_manager_team();
    m_state = 0;
    // b28d-lane6 [N-P75]：ORIG 0x0855e594 先 `mov -0x1c(%ebp),%eax`（第二实参装载）
    // 再 `movl $0x0,0xc(%esp)`（第三实参常量压栈）；旧判词「cast/NULL/具名局部
    // 3 形不可翻」**作废**。
    // [FL8PvPWorldLeague 2026-09-10] P290 生效：局部 `int`→`unsigned int` 使该实参
    // 升格 callee-saved/提前物化，复现 ORIG 的「先载第二实参、后压常量」序。
    // 本件 DIFF→AE：TU 四数 identical=44 ae=61→**62** near=1 diff=18→**17**，
    // 单函数 AE-EQ（glm_one 0 残差行）。
    _ReturnTeam(room, managerTeam, 0);
    if (managerTeam != 1)
        m_teamA = m_teamB;   // ORIG rep movsl 0x1c dwords（P83 结构整体赋值）
    m_teamA.refresh();
    m_teamB.reset();
}