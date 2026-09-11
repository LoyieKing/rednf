// ORIG logSnapShot 0x0854f4c0：logToFile(+0x10c, +0x248, false, ".snap", ..., false)。
unsigned char CLog::logSnapShot(const char* fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    unsigned char ret = logToFile(m_logFileSnap, m_logTime[7], false, ".snap", fmt,
                                  (char*)ap, false);
    return ret;
}