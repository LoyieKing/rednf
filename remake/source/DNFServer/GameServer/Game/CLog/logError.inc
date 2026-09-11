// ORIG logError 0x0854f1c2：logToFile(+0xf4, +0x218, false, ".error", ..., true)。
unsigned char CLog::logError(const char* fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    unsigned char ret = logToFile(m_logFileError, m_logTime[1], false, ".error", fmt,
                                  (char*)ap, true);
    return ret;
}