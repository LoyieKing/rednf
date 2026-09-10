void CPowerWarLog::PrintPowerWarAssultReport()          // ORIG T 0x084802cc (0x58e)
{
    // docs/class_func_reports/CPowerWarLog/PrintPowerWarAssultReport.md
    // 常量 .rodata@0x8c72fd8 = double 100.0（objdump 实测 00005940）。
    // P221/P227：ORIG 的 20 个 sibling 块 cMyTrace 槽深→浅（-0x158→-0x28，
    // 4.1「先声明更深」），4.4 对具名对象恒浅→深 ⇒ 改全表达式临时链。
    cMyTrace(__PRETTY_FUNCTION__, 0x32f, 0)(
        "-------------------------------------------------------------------------------------------------------------");
    cMyTrace(__PRETTY_FUNCTION__, 0x330, 0)(
        "|                                  POWAR WAR ASSAULT RESULT REPORT                                          |");
    cMyTrace(__PRETTY_FUNCTION__, 0x331, 0)(
        "-------------------------------------------------------------------------------------------------------------");
    cMyTrace(__PRETTY_FUNCTION__, 0x334, 0)("1. POWER WAR ATTEMPT REPORT");
    cMyTrace(__PRETTY_FUNCTION__, 0x335, 0)(
        "- POWER WAR ATTEMPT                : %d", m_attemptCount);
    cMyTrace(__PRETTY_FUNCTION__, 0x336, 0)(
        "- POWER WAR ATTEMPT SUCCESS        : %d", m_successCount);
    cMyTrace(__PRETTY_FUNCTION__, 0x337, 0)(
        "- POWER WAR ATTEMPT FAILURE        : %d", m_failureCount);
    double failureRatio = 0.0;
    if (m_attemptCount != 0)
        failureRatio = 100.0 * ((double)m_failureCount / (double)m_attemptCount);
    cMyTrace(__PRETTY_FUNCTION__, 0x340, 0)(
        "- POWER WAR ATTEMPT FAILURE RATIO  : %.2f%%", failureRatio);
    cMyTrace(__PRETTY_FUNCTION__, 0x343, 0)(
        "2. POWER WAR LOADING & P2P FAILURE REPORT");
    cMyTrace(__PRETTY_FUNCTION__, 0x344, 0)(
        "- POWER WAR LOADING TIMEOUT        : %d", m_loadingTimeout);
    cMyTrace(__PRETTY_FUNCTION__, 0x345, 0)(
        "- POWER WAR P2P TIMEOUT            : %d", m_p2pTimeoutFailure);
    cMyTrace(__PRETTY_FUNCTION__, 0x346, 0)(
        "- POWER WAR P2P BAD                : %d", m_p2pBadFailure);
    double startFailureRatio = 0.0;
    if (m_successCount != 0)
        startFailureRatio = 100.0 * ((double)m_startFailure / (double)m_successCount);
    cMyTrace(__PRETTY_FUNCTION__, 0x34f, 0)(
        "- POWER WAR FAILURE RATIO         : %.2f%%", startFailureRatio);
    cMyTrace(__PRETTY_FUNCTION__, 0x352, 0)("3. POWER WAR START SECEDE REPORT");
    cMyTrace(__PRETTY_FUNCTION__, 0x353, 0)(
        "- POWER WAR LOADING SECEDE        : %d", m_loadingTimeoutSecede);
    cMyTrace(__PRETTY_FUNCTION__, 0x354, 0)(
        "- POWER WAR P2P SECEDE            : %d", m_p2pTimeoutSecede);
    cMyTrace(__PRETTY_FUNCTION__, 0x355, 0)(
        "- POWER WAR P2P Bad               : %d", m_p2pBadSecede);
    cMyTrace(__PRETTY_FUNCTION__, 0x357, 0)(
        "4. POWER WAR SUCCESS/FAILURE INFOMATION");
    cMyTrace(__PRETTY_FUNCTION__, 0x358, 0)(
        "- SUCCESS COUNT                   : %d", m_startSuccessCount);
    cMyTrace(__PRETTY_FUNCTION__, 0x359, 0)(
        "-------------------------------------------------------------------------------------------------------------");
