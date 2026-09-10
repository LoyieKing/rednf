void WriteLoginInfo(char const* path, char const* fmt, ...)
{
    char buf[1024];
    va_list ap;
    FILE* f = fopen(path, "wb");
    if (f != 0)
    {
        va_start(ap, fmt);
        vsprintf(buf, fmt, ap);
        va_end(ap);
        fputs(buf, f);
        fclose(f);
    }
    return;
