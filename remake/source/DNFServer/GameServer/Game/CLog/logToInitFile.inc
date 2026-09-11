// ORIG logToInitFile 0x0854ef56：memset 双缓冲 + setTime + checkDay 日切；
// vsprintf；格式串变量化（bTimestamp ? "\r\n%s" : "%s"，printf 与 fprintf 共用）；
// bPrint 时行首加时间戳；返回 0/1。
int CLog::logToInitFile(FILE*& out, TimeLog_& last, char* ext,
                        char const* fmt, char* va, bool bPrint, bool bTimestamp)
{
    if (this == 0)
        return 0;
    if (out == 0)
        return 0;
    char line[0x1000];
    char body[0x1000];
    std::memset(line, 0, sizeof(line));
    std::memset(body, 0, sizeof(body));
    TimeLog_ now;
    now.setTime();
    if (out != stdout)
    {
        if (!checkDay(now, &last, out, ext, m_flag))
            return 0;
    }
    vsprintf(body, fmt, va);
    char fmtStr[16] = { 0 };
    if (bTimestamp)
        std::memcpy(fmtStr, "%s\r\n", 5);   // ORIG 0x8c9ede4（后缀 CRLF）
    else
        std::memcpy(fmtStr, "%s", 3);         // ORIG 0x8c9ed4b
    std::printf(fmtStr, body);
    if (bPrint)
    {
        std::sprintf(line, "[%02d:%02d:%02d] %s", (int)now.m_hour,
                     (int)now.m_min, (int)now.m_sec, body);
    }
    else
    {
        std::strcpy(line, body);
    }
    std::fprintf(out, fmtStr, line);
    std::fflush(out);
    return 1;
}