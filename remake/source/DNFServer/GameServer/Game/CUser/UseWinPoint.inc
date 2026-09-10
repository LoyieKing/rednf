int CUser::UseWinPoint(int winPoint, eWPSubReason reason)
{
    useWinPoint(winPoint, reason);
    if (winPoint != 0)
    {
        m_historyLog.WPSub(getPVPResultR()->m_winPoint, winPoint, reason);
    }
    return getPVPResultR()->m_winPoint;
