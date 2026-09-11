void CDeathTower::_onPrepareFinishTower()
{
    // [b28d-D13] P0 清绑（N96）：key 改内联实参（P24），ORIG 0x8467e00 mov %eax,%ebx
    // 为 4.4 对内联实参自然升格 callee-saved（P200/P226）。
    G_TimerQueue()->InsertTimer(
        (TimerEntry::OBJ_TYPE)6, m_idx, (TIMER_MESSAGE)0x4a, 5,
        _genTimerKey((TIMER_MESSAGE)0x4a), 0x4a);
}