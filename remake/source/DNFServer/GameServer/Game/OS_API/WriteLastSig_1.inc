// ORIG 0x0858c5f4 _ZN6OS_API12WriteLastSigEPKcS1_jiiPKviiiiii：
// 512 栈缓冲清零 → sprintf "./log/%s/%s" → NumberToString(u,0) →
// CMyFileLog(__PRETTY_FUNCTION__, 0x4e) → log(path, "U:%s ...", us, 后续 9 参)。
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
}