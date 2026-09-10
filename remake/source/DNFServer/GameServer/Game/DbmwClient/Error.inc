void DbmwClient::Error(int level, int useErrno, char* fmt, ...)
{
    // ORIG @0x08120c18：捕获 errno → vsprintf → 可选追加 ": strerror\n"
    // → fputs(stderr) → 还原 errno。level 仅占位（ORIG 未消费）。
    (void)level;
    char buf[200];
    va_list args;
    va_start(args, fmt);
    int savedErrno = errno;
    vsprintf(buf, fmt, args);
    va_end(args);

    if (useErrno != 0)
    {
        sprintf(buf + strlen(buf), ": %s\n", strerror(savedErrno));
    }
    else
    {
        strcat(buf, "\n");
    }
    fputs(buf, stderr);
    errno = savedErrno;
