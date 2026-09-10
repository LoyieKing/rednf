void WarRoom::ChangeState(WARROOM_STATE state)
{
    // ORIG 086bb53c：je 086bb57f(nop;fall leave) 早退岛 + body 尾 jmp 086bb580。
    // 技巧6 早退形（正向 if!= 在 4.4 会吞尾岛）。
    if (m_state == state)
        return;
    OnLeaveState(state);
    m_state = state;
    OnEnterState();
