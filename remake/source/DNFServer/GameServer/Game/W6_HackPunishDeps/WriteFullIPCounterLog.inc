void CAutoPunishRuleHackTypeMgr::WriteFullIPCounterLog(CHackAnalyzer* analyzer,
                                                       int a, int b, int c, int d,
                                                       int e, char* ip)
{
    CUser* user = analyzer->getUserP();
    if (user != 0)
    {
        user->m_historyLog.FullIPCounterLog(a, b, c, d, e, ip);
    }
