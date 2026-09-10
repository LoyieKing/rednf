void getDirectoryPath(std::string& out, const char* fmt, ...)
{
    char buf[512];
    va_list ap;
    va_start(ap, fmt);
    std::vsnprintf(buf, 0x200, fmt, ap);
    va_end(ap);

    int i = 0;
    i = static_cast<int>(std::strlen(buf)) - 1;
    while (i >= 0)
    {
        if (buf[i] == '\\')
            break;
        if (buf[i] == '/')
            break;
        --i;
    }
    buf[i + 1] = '\0';
    out = buf;
