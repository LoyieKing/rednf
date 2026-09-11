void CUser::gainWinPoint(int winPoint, eWPAddReason reason)
{
    // ORIG _ZN5CUser12gainWinPointEi12eWPAddReason @0x0864fd2c。
    if (winPoint != 0)
    {
        // ORIG 0x864fd2c：call 结果留 %eax 直读直存（`mov 0x58(%eax),%edx; add; mov %edx,0x58(%eax)`
        // 无具名槽——链式形；具名 wpW/wpR 反产 spill 槽撑帧 0x28）。
        getPVPResultW()->m_winPoint += winPoint;
        m_historyLog.WPAdd(getPVPResultR()->m_winPoint, winPoint, reason);
    }
}