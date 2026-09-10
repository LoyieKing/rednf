void TraceLog::sysNSL_LOG_date_ch()
{
    // ORIG DWARF 声明序（decl 220/221/222/223/225）：数组→orifname→st→size/rst/i→slock
    char logdname[4096];
    char renfname[4096];
    char* orifname;
    struct stat st;
    int size;
    int rst;
    int i;
    TScopedLock<TThreadLock<ThreadLock_linux> > slock(lockLog);
    time_t now;
    tm* tm_now;
    int ret;
    // ORIG: 赋值在条件内（mov rst; shr $0x1f; test; je）
    if ((rst = stat(logfname, &st)) < 0)
    {
        errorLog("sysNSL_LOG_date_ch stat() errmsg[%s(%d)] [%s]", strerror(errno), errno, logfname);
        exit(1);
    }
    // 原始：access 失败分支在成功分支之前（callset 顺序）
    if (access(logfname, 0) != 0)
    {
        if (bNeedToLeaveErrorLogForOverSize != false)
        {
            errorLog("sysNSL_LOG_date_ch access fail : errmsg[%s(%d)] : logfilename[%s]", strerror(errno), errno, logfname);
            bNeedToLeaveErrorLogForOverSize = false;
        }
    }
    else
    {
        if (0x1388000 < st.st_size)
        {
            bChangedDataForLog = true;
        }
        // 原始：if (!bChangedDataForLog) 跳过；否则旋转日志
        if (bChangedDataForLog == false)
        {
        }
        else
        {
            bChangedDataForLog = false;
            sprintf(logdname, "%s/old_log", G_Script()->findCharValue(0, 1));
            if ((rst = stat(logdname, &st)) < 0)
            {
                if (errno != ENOENT)
                {
                    errorLog("sysNSL_LOG_date_ch stat() errmsg[%s(%d)]", strerror(errno), errno);
                    return;
                }
                if (MakeDir(logdname) < 0)
                {
                    errorLog("sysNSL_LOG_date_ch mkdir() errmsg[%s(%d)]", strerror(errno), errno);
                    return;
                }
            }
            // 原始：close 结果存局部再与 0 比较
            ret = close(logfd);
            if (ret != 0)
            {
                printf("file close error ('%s'), fd=%d\n", strerror(errno), logfd);
            }
            ret = close(errfd);
            if (ret != 0)
            {
                printf("file close error ('%s'), fd=%d\n", strerror(errno), errfd);
            }
            ret = close(statfd);
            if (ret != 0)
            {
                printf("file close error ('%s'), fd=%d\n", strerror(errno), statfd);
            }
            for (i = 0; i < 3; i = i + 1)
            {
                if (i == 0)
                {
                    now = time(NULL);
                    tm_now = localtime(&now);
                }
                if (i == 0)
                {
                    orifname = logfname;
                    sprintf(renfname, "%s/Info_%sB#%02d%02d%02d_%02d%02d%02d.log", logdname, mpServiceIdentity, tm_now->tm_year % 100, tm_now->tm_mon + 1, tm_now->tm_mday, tm_now->tm_hour, tm_now->tm_min, tm_now->tm_sec);
                }
                else if (i == 1)
                {
                    orifname = errfname;
                    sprintf(renfname, "%s/Error_%sB#%02d%02d%02d_%02d%02d%02d.log", logdname, mpServiceIdentity, tm_now->tm_year % 100, tm_now->tm_mon + 1, tm_now->tm_mday, tm_now->tm_hour, tm_now->tm_min, tm_now->tm_sec);
                }
                else
                {
                    orifname = statfname;
                    sprintf(renfname, "%s/Stat_%sB#%02d%02d%02d_%02d%02d%02d.log", logdname, mpServiceIdentity, tm_now->tm_year % 100, tm_now->tm_mon + 1, tm_now->tm_mday, tm_now->tm_hour, tm_now->tm_min, tm_now->tm_sec);
                }
                if (rename(orifname, renfname) < 0)
                {
                    set_fd(0x148);
                    errorLog("sysNSL_LOG_date_ch rename() errmsg[%s(%d)]", strerror(errno), errno);
                }
            }
            set_fd(0x14e);
        }
    }
