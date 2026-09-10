void CPowerWarLog::Reset()                              // ORIG T 0x08480232 (0x9a)
{
    // ORIG 0x08480232：15 条独立 `mov 0x8(%ebp),%eax; movl $0x0,disp(%eax)`
    // （逐字段重读 this、无循环变量/无栈帧）⇒ 原码是逐成员赋值（P54/P171）。
    m_attemptCount = 0;
    m_successCount = 0;
    m_failureCount = 0;
    m_loadingTimeout = 0;
    m_loadingTimeoutSecede = 0;
    m_p2pTimeoutFailure = 0;
    m_p2pTimeoutSecede = 0;
    m_p2pBadFailure = 0;
    m_p2pBadSecede = 0;
    m_startSuccess = 0;
    m_startFailure = 0;
    m_startSuccessCount = 0;
    m_field30 = 0;
    m_totalJoinAssaultUser = 0;
    m_totalLeaveAssaultUser = 0;
