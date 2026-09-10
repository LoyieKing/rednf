void TraceLog::errorLog(const char* msg, ...)
{
    va_list ap;
    va_start(ap, msg);
    char tmpbuf[2048];
    // 原始：同 sysLog，VsnPrintf(tmpbuf, ...) + while (tmpbuf[len-1]=='\n')
    int len = VsnPrintf(tmpbuf, 0x800, msg, ap);
    va_end(ap);
    get_time(cur_date, cur_time);
    while (tmpbuf[len - 1] == '\n')
    {
        tmpbuf[len - 1] = '\0';
        len = len - 1;
    }
    char buf[2088];
    len = SnPrintf(buf, 0x2028, (char*)"%s %s : %s\n", cur_date, cur_time, tmpbuf);
    write(errfd, buf, len);
