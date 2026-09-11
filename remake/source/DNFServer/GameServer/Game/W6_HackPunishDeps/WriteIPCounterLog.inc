// ----------------------------------------------------------------------------
// WriteIPCounterLog —— ORIG T 0x080f967a (0x57)：getUserP 非空时转发
// cUserHistoryLog(+0x79700)::IPCounterLog(...)。
// ----------------------------------------------------------------------------
void CAutoPunishRuleHackTypeMgr::WriteIPCounterLog(CHackAnalyzer* analyzer, int a,
                                                   int b, int c, int d, int e,
                                                   char* ip)
{
    CUser* user = analyzer->getUserP();
    if (user != 0)
    {
        user->m_historyLog.IPCounterLog(a, b, c, d, e, ip);
    }
}