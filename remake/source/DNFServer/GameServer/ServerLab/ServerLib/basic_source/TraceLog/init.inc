int TraceLog::init(char* ServiceName, char* ServiceIdentity)
{
    mpServiceIdentity = ServiceIdentity;
    memset(logfname, 0, 0x100);
    memset(errfname, 0, 0x100);
    memset(statfname, 0, 0x100);
    sprintf(logfname, "%s/Info_%s.log", G_Script()->findCharValue(0, 1), ServiceIdentity);
    sprintf(errfname, "%s/Error_%s.log", G_Script()->findCharValue(0, 1), ServiceIdentity);
    sprintf(statfname, "%s/Stat_%s.log", G_Script()->findCharValue(0, 1), ServiceIdentity);
    if (access(logfname, 0) == 0)
    {
        char backup_name[1024];
        struct stat st;
        sprintf(backup_name, "%s/old_log", G_Script()->findCharValue(0, 1));
        if (stat(backup_name, &st) < 0)
        {
            if (MakeDir(backup_name) < 0)
            {
                return -1;
            }
        }
        time_t log_now = time(NULL);
        tm* tm_now = localtime(&log_now);
        sprintf(backup_name, "%s/old_log/Info_%s#%02d%02d%02d_%02d%02d%02d.log", G_Script()->findCharValue(0, 1), ServiceIdentity, tm_now->tm_year % 100, tm_now->tm_mon + 1, tm_now->tm_mday, tm_now->tm_hour, tm_now->tm_min, tm_now->tm_sec);
        rename(logfname, backup_name);
        sprintf(backup_name, "%s/old_log/Error_%s#%02d%02d%02d_%02d%02d%02d.log", G_Script()->findCharValue(0, 1), ServiceIdentity, tm_now->tm_year % 100, tm_now->tm_mon + 1, tm_now->tm_mday, tm_now->tm_hour, tm_now->tm_min, tm_now->tm_sec);
        rename(errfname, backup_name);
        sprintf(backup_name, "%s/old_log/Stat_%s#%02d%02d%02d_%02d%02d%02d.log", G_Script()->findCharValue(0, 1), ServiceIdentity, tm_now->tm_year % 100, tm_now->tm_mon + 1, tm_now->tm_mday, tm_now->tm_hour, tm_now->tm_min, tm_now->tm_sec);
        rename(statfname, backup_name);
    }
    setMask();
    if (set_fd(99) < 0)
    {
        return -1;
    }
    return 0;
