int ss_sprintf(char (&buffer)[N], const char* format, ...)
{
    // ORIG：va_list ap 局部（槽往返）+ 返回值具名局部（P0）
    va_list ap;
    va_start(ap, format);
    int ret = ss_vsprintf(buffer, N, format, reinterpret_cast<char*>(ap));
    va_end(ap);
    return ret;
