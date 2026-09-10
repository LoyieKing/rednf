void CLeagueMatch::OnEndFight(PvP_Room* room)
{
    int managerTeam = room->get_manager_team();
    m_state = 0;
    // b28d-lane6 [N-P75]：ORIG 0x0855e594 先 `mov -0x1c(%ebp),%eax`（第二实参装载）
    // 再 `movl $0x0,0xc(%esp)`（第三实参常量压栈）；4.4 恒先压常量。cast/NULL/
    // 具名局部 3 形实测均不可翻（P75 跨版本实参装载时刻）。
    _ReturnTeam(room, managerTeam, 0);
    if (managerTeam != 1)
        m_teamA = m_teamB;   // ORIG rep movsl 0x1c dwords（P83 结构整体赋值）
    m_teamA.refresh();
    m_teamB.reset();
