void CExceptLog::fPrintTextFile(char* name, char* fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    vsprintf(reinterpret_cast<char*>(this), fmt, ap);
    va_end(ap);

    PrintTextFile(name, reinterpret_cast<char*>(this));
