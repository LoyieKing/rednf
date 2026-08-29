#include "OS_API.h"

#include <cstdio>
#include <cstdlib>
#include <cstdarg>

#include "CMyFileLog.h"
char* NumberToString(unsigned int value, int radix);   // 定义 LinkFinalDeps.cpp/ServerCommon

// 5 个 ORIG 符号的实现（df_game_r 0x0858c53a..0x0858c789）。
// 其余 5 符号（GetDateTimeTick/snprintf/Sleep/WriteLog/uSleep）由既有文件以
// namespace OS_API 定义，声明见 OS_API.h 与 CInventory.h。
// 全部为自由函数（ORIG mangled 与 namespace 函数一致，均无 this 参与）。

namespace OS_API
{

// ORIG 0x0858c53a _ZN6OS_API4ExitEi：直接 exit。
void Exit(int exitCode)
{
    exit(exitCode);
}

// ORIG 0x0858c6d4 _ZN6OS_API17WriteLastBookmarkEPKci：
// fopen(path,"wb") 成功则 fprintf "bm:%d" 后 fclose。
void WriteLastBookmark(char const* path, int value)
{
    FILE* f = fopen(path, "wb");
    if (f != 0)
    {
        fprintf(f, "bm:%d", value);
        fclose(f);
    }
}

// ORIG 0x0858c54c _ZN6OS_API12WriteLastSigEPKcS1_iiPKvi：
// 256 栈缓冲清零 → sprintf "./log/%s/%s" → fopen("wb") →
// fprintf "K:%d C:%d P:%p SZ:%d\n" → fclose。
void WriteLastSig(char const* dir, char const* name, int k, int c, void const* p, int sz)
{
    char path[256] = { '\0' };
    sprintf(path, "./log/%s/%s", dir, name);
    FILE* f = fopen(path, "wb");
    if (f != 0)
    {
        fprintf(f, "K:%d C:%d P:%p SZ:%d\n", k, c, p, sz);
        fclose(f);
    }
}

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

// ORIG 0x0858c71e _ZN6OS_API14WriteLoginInfoEPKcS1_z：
// fopen(path,"wb") 成功则 vsprintf 到 1024 栈缓冲 → fputs → fclose。
void WriteLoginInfo(char const* path, char const* fmt, ...)
{
    char buf[1024];
    va_list ap;
    FILE* f = fopen(path, "wb");
    if (f != 0)
    {
        va_start(ap, fmt);
        vsprintf(buf, fmt, ap);
        va_end(ap);
        fputs(buf, f);
        fclose(f);
    }
    return;
}

}  // namespace OS_API
