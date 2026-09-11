// ----------------------------------------------------------------------------
// WriteFullIPCounterLog —— ORIG T 0x080f96d2 (0x57)：转发
// cUserHistoryLog(+0x79700)::FullIPCounterLog(...)。
// ----------------------------------------------------------------------------
void CAutoPunishRuleHackTypeMgr::WriteFullIPCounterLog(CHackAnalyzer* analyzer,
                                                       int a, int b, int c, int d,
                                                       int e, char* ip)
{
    CUser* user = analyzer->getUserP();
    if (user != 0)
    {
        user->m_historyLog.FullIPCounterLog(a, b, c, d, e, ip);
    }
}