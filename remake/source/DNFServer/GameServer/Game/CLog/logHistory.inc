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
