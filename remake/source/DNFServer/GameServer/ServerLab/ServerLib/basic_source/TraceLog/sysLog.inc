void TraceLog::sysLog(int flag, const char* msg, ...)
{
    bool other_file_log = false;
    if (flag == 9)
    {
        other_file_log = true;
    }
    else if (get_mask(flag) == 0)
    {
        return;
    }
    va_list ap;
    va_start(ap, msg);
    char tmpbuf[2048];
    // 原始：VsnPrintf 写到 tmpbuf 基址（非 tmpbuf+1）
    int len = VsnPrintf(tmpbuf, 0x800, msg, ap);
    va_end(ap);
    get_time(cur_date, cur_time);
    // 原始：while (tmpbuf[len-1] == '\n') { tmpbuf[len-1]=0; len--; }
    while (tmpbuf[len - 1] == '\n')
    {
        tmpbuf[len - 1] = '\0';
        len = len - 1;
    }
    char buf[2088];
    len = SnPrintf(buf, 0x2028, (char*)"%s %s : %s\n", cur_date, cur_time, tmpbuf);
    if (len < 0)
    {
        return;
    }
    // 原始：if (write_to_logserver) SendLogMsg 在前，else 本地写
    if (write_to_logserver != false)
    {
        logSendThread_->SendLogMsg(buf, len);
    }
    else
    {
        sysNSL_LOG_date_ch();
        if (logfd < 1)
        {
            puts("syslog fd error");
        }
        else if (other_file_log != false)
        {
            write(statfd, buf, len);
        }
        else
        {
            if (flag == 7)
            {
                write(errfd, buf, len);
            }
            write(logfd, buf, len);
        }
    }
