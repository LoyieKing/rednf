unsigned char CLog::logConsole(const char* fmt, ...)
{
    if (this == 0)
        return 0;
    TimeLog_ now;
    now.setTime();
    va_list ap;
    va_start(ap, fmt);
    char line[4096];
    char body[4096];
    vsprintf(body, fmt, ap);
    std::sprintf(line, "[%02d:%02d:%02d] %s", (int)now.m_hour,
                 (int)now.m_min, (int)now.m_sec, body);
    std::printf("%s\r\n", line);
    return 1;
