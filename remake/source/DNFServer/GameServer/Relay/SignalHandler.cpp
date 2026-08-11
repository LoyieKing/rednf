// df_relay_r — Neof_* 信号/守护函数（GCC 4.1.2, 无 DWARF）
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/resource.h>

#include "SignalHandler.h"
#include "Helper.h"
#include "ScriptData.h"
#include "LinuxService.h"

void Neof_SignalLog(const char* msg)
{
    time_t t = get_ms_tick() / 1000;
    struct tm* tm_now = localtime(&t);
    char filename[256] = {0};
    snprintf(filename, 0x100, "./log/Relay%4d%02d%02d_T%d.log",
             tm_now->tm_year + 1900, tm_now->tm_mon + 1, tm_now->tm_mday,
             G_ScriptData()->mPortTcp);
    FILE* f = fopen(filename, "a+");
    if (f != 0)
    {
        fprintf(f, "[%02d/%02d/%02d %02d:%02d:%02d] Signal occured: %s\n",
                tm_now->tm_year + 1900, tm_now->tm_mon + 1, tm_now->tm_mday,
                tm_now->tm_hour, tm_now->tm_min, tm_now->tm_sec, msg);
        fclose(f);
    }
}

bool Neof_registerSignalHandler(int sig, void (*handler)(int))
{
    struct sigaction sa;
    struct sigaction old;
    sa.sa_handler = (__sighandler_t)handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    if (sig == 0xe)
    {
        sa.sa_flags |= 0x20000000;
    }
    else
    {
        sa.sa_flags |= 0x10000000;
    }
    if (sigaction(sig, &sa, &old) < 0)
    {
        printf("%d\xb9\xf8 signal \xb5\xee\xb7\xcf \xbd\xc7\xc6\xd0\n", sig);
        return 0;
    }
    return 1;
}

void Neof_dumpCoreFile()
{
    abort();
}

void Neof_SignalHandler(int sig)
{
    switch (sig)
    {
    case 0xf:
        Neof_SignalLog("SIGTERM - LinuxService::getInstance()->controlStop()");
        LinuxService::getInstance()->controlStop();
        break;
    case 10:
        Neof_SignalLog("SIGUSR1 - LinuxService::getInstance()->controlPause()");
        LinuxService::getInstance()->controlPause();
        break;
    case 6:
    case 0xb:
        Neof_SignalLog("SIGSEGV/SIGABRT - Neof_dumpCoreFile()");
        Neof_dumpCoreFile();
        break;
    case 8:
        Neof_SignalLog("SIGFPE - Neof_dumpCoreFile()");
        Neof_dumpCoreFile();
        break;
    case 4:
    case 7:
    case 0x10:
    case 0x17:
    case 0x18:
    case 0x19:
    case 0x1f:
        Neof_SignalLog(
            "SIGILL/SIGBUS/SIGSTKFLT/SIGURG/SIGXCPU/SIGXFSZ/SIGSYS - Neof_dumpCoreFile(), exit(-1)");
        Neof_dumpCoreFile();
        exit(-1);
    }
}

bool Neof_registerSignalHandlers()
{
    bool ret = false;
    if (!Neof_registerSignalHandler(0xf, Neof_SignalHandler)) return false;
    if (!Neof_registerSignalHandler(10, Neof_SignalHandler)) return false;
    ret = Neof_registerSignalHandler(0xd, (void (*)(int))1);
    if (!ret) return false;
    ret = Neof_registerSignalHandler(0xb, Neof_SignalHandler);
    if (!ret) return false;
    ret = Neof_registerSignalHandler(8, Neof_SignalHandler);
    if (!ret) return false;
    ret = Neof_registerSignalHandler(4, Neof_SignalHandler);
    if (!ret) return false;
    ret = Neof_registerSignalHandler(7, Neof_SignalHandler);
    if (!ret) return false;
    ret = Neof_registerSignalHandler(0x10, Neof_SignalHandler);
    if (!ret) return false;
    ret = Neof_registerSignalHandler(0x17, Neof_SignalHandler);
    if (!ret) return false;
    ret = Neof_registerSignalHandler(0x18, Neof_SignalHandler);
    if (!ret) return false;
    ret = Neof_registerSignalHandler(0x19, Neof_SignalHandler);
    if (!ret) return false;
    ret = Neof_registerSignalHandler(0x1f, Neof_SignalHandler);
    if (!ret) return false;
    return true;
}

void Neof_setCoreLimit()
{
    struct rlimit rl;
    int r = getrlimit(RLIMIT_CORE, &rl);
    rl.rlim_cur = 0xffffffff;
    r = setrlimit(RLIMIT_CORE, &rl);
}

bool Neof_sendTerminateSignal()
{
    puts("called Neof_sendTerminateSignal");
    char path[30] = {0};
    char buf[30];
    int r;
    int pid;
    snprintf(path, 0x1e, "pid/%s.pid",
             LinuxService::getInstance()->getPIDFileName());
    FILE* f = fopen(path, "r");
    if (f == 0)
    {
        printf("%s process id file open \xbd\xc7\xc6\xd0\n", path);
        return false;
    }
    fgets(buf, 0x1e, f);
    pid = atoi(buf);
    if (pid < 1)
    {
        fclose(f);
        printf("%d\xb9\xf8\xc0\xc7 \xc0\xdf\xb8\xf8\xb5\xc8 process id\n", pid);
        return false;
    }
    r = kill(pid, 0xf);
    if (r < 0)
    {
        fclose(f);
        printf("[ret = %d ]called Neof_sendTerminateSignal() : %d\xb9\xf8 process\xb7\xce \xc1\xbe\xb7\xe1 signal \xbc\xdb\xbd\xc5 \xbd\xc7\xc6\xd0", r, pid);
        return false;
    }
    fclose(f);
    return true;
}

void Neof_sendSuspendSignal()
{
    char path[30] = {0};
    char buf[30];
    int r;
    int pid;
    snprintf(path, 0x1e, "pid/%s",
             LinuxService::getInstance()->getPIDFileName());
    FILE* f = fopen(path, "r");
    if (f == 0)
    {
        printf("%s process id file open \xbd\xc7\xc6\xd0\n", path);
        return;
    }
    fgets(buf, 0x1e, f);
    pid = atoi(buf);
    if (pid < 1)
    {
        fclose(f);
        printf("%d\xb9\xf8\xc0\xc7 \xc0\xdf\xb8\xf8\xb5\xc8 process id\n", pid);
        return;
    }
    r = kill(pid, 10);
    if (r < 0)
    {
        fclose(f);
        printf("%d\xb9\xf8 process\xb7\xce \xc1\xbe\xb7\xe1 signal \xbc\xdb\xbd\xc5 \xbd\xc7\xc6\xd0", pid);
        return;
    }
    printf("SEND SUSPEND SIGNAL TO %d\n", pid);
    fclose(f);
}
