unsigned char CLog::logError(const char* fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    unsigned char ret = logToFile(m_logFileError, m_logTime[1], false, ".error", fmt,
                                  (char*)ap, true);
    return ret;
