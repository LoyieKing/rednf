unsigned char CLog::logCritical(const char* fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    unsigned char ret = logToFile(m_logFileCri, m_logTime[5], false, ".cri", fmt,
                                  (char*)ap, true);
    return ret;
