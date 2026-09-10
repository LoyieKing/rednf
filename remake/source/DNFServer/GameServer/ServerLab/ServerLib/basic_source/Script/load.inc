bool Script::load(char* filename)
{
    TScopedLock<TThreadLock<ThreadLock_linux> > slock(mLoadScriptLock);
    char buf[0x401];
    memset(buf, 0, 0x401);
    Clear();
    depth = 0;
    fp = fopen(filename, "rb");
    if (fp == NULL)
    {
        return false;
    }
    bool ret;
    for (;;)
    {
        ret = fgetln(fp, buf);
        parse(buf);
        if (!(ret == true))
        {
            break;
        }
    }
    fclose(fp);
    return true;
