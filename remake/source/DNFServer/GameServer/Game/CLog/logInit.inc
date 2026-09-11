// ORIG logInit 0x0854f520：logToInitFile(+0x108, +0x240, ".init", fmt, va,
// bForceOpen, bAppend)。
unsigned char CLog::logInit(bool bForceOpen, bool bAppend, const char* fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    unsigned char ret = logToInitFile(m_logFileInit, m_logTime[6], ".init", fmt,
                                      (char*)ap, bForceOpen, bAppend);
    return ret;
}