unsigned char CLog::logNotice(const char* fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    unsigned char ret = logToFile(m_logFileNotice, m_logTime[0], false, ".log", fmt,
                                  (char*)ap, true);
    return ret;
