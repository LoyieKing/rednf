int CLog::logToFile(FILE*& out, TimeLog_& last, bool bPrint, char* ext,
                    char const* fmt, char* va, bool bTimestamp)
{
    if (this == 0)
        return 0;
    if (out == 0)
        return 0;
    TimeLog_ now;
    now.setTime();
    if (out != stdout)
    {
        if (!checkDay(now, &last, out, ext, m_flag))
            return 0;
    }
    char line[4096];
    char body[4096];
    vsprintf(body, fmt, va);
    if (bTimestamp)
    {
        std::sprintf(line, "[%02d:%02d:%02d] %s", (int)now.m_hour,
                     (int)now.m_min, (int)now.m_sec, body);
        std::fprintf(out, "%s\r\n", line);
    }
    else
    {
        std::fprintf(out, "%s\r\n", body);
    }
    std::fflush(out);
    if (bPrint)
    {
        if (bTimestamp)
            std::printf("%s\r\n", line);
        else
            std::printf("%s\r\n", body);
    }
    return 1;
