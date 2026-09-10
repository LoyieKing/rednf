void CLog::openLogFile(TimeLog_ t, FILE*& out, char* ext, int flag)
{
    char path[4096];
    char buf[4096];
    if (flag != 0)
    {
        std::sprintf(buf, "%s%04d%02d%02d-%02d", m_logFile,
                     (int)(short)t.m_year, (int)t.m_mon, (int)t.m_day,
                     (int)t.m_hour);
    }
    else
    {
        std::sprintf(buf, "%s%04d%02d%02d", m_logFile,
                     (int)(short)t.m_year, (int)t.m_mon, (int)t.m_day);
    }
    std::strncpy(path, m_initFile, 0x1000);
    std::strncat(path, buf, 0x1000);
    std::strncat(path, ext, 0x1000);
    if ((out = std::fopen(path, "ab")) == 0)
        out = stdout;
