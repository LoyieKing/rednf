// ORIG logConsole 0x0854f414：this 空返回 0；setTime + vsprintf +
// "[hh:mm:ss]" 前缀 + printf("%s\r\n")，返回 1。
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
}