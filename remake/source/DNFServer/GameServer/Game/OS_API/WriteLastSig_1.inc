void WriteLastSig(char const* dir, char const* name, unsigned int u,
                  int k, int c, void const* p,
                  int tq, int dq, int nq, int dbq, int logdbq, int extra)
{
    char path[512] = { '\0' };
    sprintf(path, "./log/%s/%s", dir, name);
    register char* us = NumberToString(u, 0);
    CMyFileLog log(__PRETTY_FUNCTION__, 0x4e);
    log(path,
        "U:%s K:%d C:%d P:%p SZ:%d TimerQ:%d DispatchQ:%d NetworkQ:%d DBQ:%d, LOGDBQ:%d\n",
        us, k, c, p, tq, dq, nq, dbq, logdbq, extra);
