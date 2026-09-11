// _ZN10CSimpleLog8WriteLogEPKcz @ 0x0854fcb8
void CSimpleLog::WriteLog(char const* fmt, ...)
{
    char buf[4096];
    memset(buf, 0, 0x1000);
    va_list ap;
    va_start(ap, fmt);
    vsprintf(buf, fmt, ap);
    va_end(ap);
    _WriteLog(buf);
}