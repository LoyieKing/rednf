int ss_sprintf(char (&buffer)[N], const char* format, ...)
{
    std::va_list ap;
    va_start(ap, format);
    int n = ss_vsprintf(buffer, N, format, ap);
    va_end(ap);
    return n;
}