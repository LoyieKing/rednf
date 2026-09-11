// ORIG logHistory 0x0854f2e2：this/历史句柄(+0x100) 空返回 0；非 stdout 时
// setTime + checkDay(+0x230, +0x100, ".history", +0x254) 日切；
// vsprintf + strlen 空循环 + fprintf("%s\r\n") + fflush，返回 1。
int CLog::logHistory(const char* fmt, char* vaArgs)
{
    if (this == 0)
        return 0;
    if (m_logFileHistory == 0)
        return 0;
    if (m_logFileHistory != stdout)
    {
        TimeLog_ now;
        now.setTime();
        if (!checkDay(now, &m_logTime[4], m_logFileHistory, ".history",
                     m_initFlag))
            return 0;
    }
    char body[4096];
    vsprintf(body, fmt, vaArgs);
    int len = (int)std::strlen(body);
    int i;
    for (i = 0; i < len; ++i)
    {
    }
    std::fprintf(m_logFileHistory, "%s\r\n", body);
    std::fflush(m_logFileHistory);
    return 1;
}