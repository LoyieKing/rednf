// ORIG logMoney 0x0854f282：logToFile(+0xfc, +0x228, false, ".money", ..., true)。
unsigned char CLog::logMoney(const char* fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    unsigned char ret = logToFile(m_logFileMoney, m_logTime[3], false, ".money", fmt,
                                  (char*)ap, true);
    return ret;
}