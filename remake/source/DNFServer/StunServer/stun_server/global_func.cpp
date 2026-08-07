#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <dirent.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "global_func.h"

bool check_argv(int argc, char** argv)
{
    if (argc != 2)
    {
        return false;
    }
    char* order = argv[1];
    if (strcmp(order, "start") == 0)
    {
        if (check_pid_file())
        {
            puts("Process is already operated.");
            return false;
        }
        daemon_init();
        return true;
    }
    else if (strcmp(order, "stop") == 0)
    {
        send_term_signal();
        return false;
    }
    else if (strcmp(order, "test") == 0)
    {
        return true;
    }
    return false;
}

bool check_pid_file()
{
    if (access("./pid/udp_server.pid", 0) == 0)
    {
        return true;
    }
    return false;
}

int daemon_init()
{
    pid_t pid = fork();
    if (pid < 0)
    {
        return -1;
    }
    if (pid != 0)
    {
        exit(0);
    }
    setsid();
    chdir("./");
    umask(0);
    save_pid();
    return 0;
}

bool save_pid()
{
    int fd = open("./pid/udp_server.pid", 0x42, 0x1a4);
    if (fd < 0)
    {
        return false;
    }

    char buf[512];
    memset(buf, 0, 512);
    sprintf(buf, "%ld\n", (long)getpid());
    ssize_t write_byte = write(fd, buf, strlen(buf));
    if (write_byte < 0)
    {
        close(fd);
        return false;
    }
    return true;
}

void send_term_signal()
{
    FILE* fp = fopen("./pid/udp_server.pid", "r");
    if (fp == NULL)
    {
        puts("process id file open fail");
    }
    else
    {
        pid_t pid;
        fscanf(fp, "%d", &pid);
        if (pid < 1)
        {
            fclose(fp);
            printf("wrong process id (%d)\n", pid);
        }
        else
        {
            int ret = kill(pid, 15);
            if (ret < 0)
            {
                fclose(fp);
                printf("fail to terminate process (process id: %d)\n", pid);
            }
            else
            {
                fclose(fp);
            }
        }
    }
}

void rdar_terminate(int argv)
{
    if (remove("./pid/udp_server.pid") == -1)
    {
        write_log("FAIL TO DELETE PID FILE ERROR", errno);
    }
    if (argv == 0)
    {
        write_log("STUN PROCESS IS TERMINATED", 0);
    }
    else
    {
        write_log("STUN PROCESS IS ABNORMALLY TERMINATED", 0);
    }
    exit(argv);
}

bool regist_signal(int sig, handler func_name)
{
    struct sigaction act;
    struct sigaction oact;

    act.sa_handler = func_name;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    if (sig == SIGALRM)
    {
        act.sa_flags |= 0x20000000;
    }
    else
    {
        act.sa_flags |= 0x10000000;
    }
    if (sigaction(sig, &act, &oact) < 0)
    {
        printf("fail to regist signal (signal id: %d)\n", sig);
        return false;
    }
    return true;
}

void signal_handler(int sig)
{
    switch (sig)
    {
        case SIGTERM:
            rdar_terminate(0);
            break;
        case SIGSEGV:
            write_log("Signal(SIGSEGV) occured", sig);
            rdar_terminate(-1);
            break;
        case SIGABRT:
            write_log("Signal(SIGABRT) occured", sig);
            rdar_terminate(-1);
            break;
        case SIGFPE:
            write_log("Signal(SIGFPE) occured", sig);
            rdar_terminate(-1);
            break;
        case SIGILL:
            write_log("Signal(SIGILL) occured", sig);
            rdar_terminate(-1);
            break;
        case SIGBUS:
            write_log("Signal(SIGBUS) occured", sig);
            rdar_terminate(-1);
            break;
        case SIGSTKFLT:
            write_log("Signal(SIGSTKFLT) occured", sig);
            rdar_terminate(-1);
            break;
        case SIGURG:
            write_log("Signal(SIGURG) occured", sig);
            rdar_terminate(-1);
            break;
        case SIGXCPU:
            write_log("Signal(SIGXCPU) occured", sig);
            rdar_terminate(-1);
            break;
        case SIGXFSZ:
            write_log("Signal(SIGXFSZ) occured", sig);
            rdar_terminate(-1);
            break;
        case SIGSYS:
            write_log("Signal(SIGSYS) occured", sig);
            rdar_terminate(-1);
            break;
    }
}

bool regist_signals()
{
    if (!regist_signal(SIGTERM, signal_handler))
    {
        return false;
    }
    if (!regist_signal(SIGSEGV, signal_handler))
    {
        return false;
    }
    if (!regist_signal(SIGABRT, signal_handler))
    {
        return false;
    }
    if (!regist_signal(SIGFPE, signal_handler))
    {
        return false;
    }
    if (!regist_signal(SIGILL, signal_handler))
    {
        return false;
    }
    if (!regist_signal(SIGBUS, signal_handler))
    {
        return false;
    }
    if (!regist_signal(SIGSTKFLT, signal_handler))
    {
        return false;
    }
    if (!regist_signal(SIGURG, signal_handler))
    {
        return false;
    }
    if (!regist_signal(SIGXCPU, signal_handler))
    {
        return false;
    }
    if (!regist_signal(SIGXFSZ, signal_handler))
    {
        return false;
    }
    if (!regist_signal(SIGSYS, signal_handler))
    {
        return false;
    }
    return true;
}

void make_dir(const char* szdir_name)
{
    DIR* p_dir = opendir(szdir_name);
    if (p_dir == NULL)
    {
        mkdir(szdir_name, 0777);
    }
}

void write_log(const char* pLog, int argv)
{
    time_t currentTime = time(NULL);
    tm* currentTm = localtime(&currentTime);

    char logFileName[256];
    memset(logFileName, 0, sizeof(logFileName));
    sprintf(logFileName, "./log/STUN%4d%02d%02d.log", currentTm->tm_year + 1900, currentTm->tm_mon + 1, currentTm->tm_mday);

    FILE* pFile = fopen(logFileName, "a+");
    if (pFile != NULL)
    {
        if (argv != 0)
        {
            fprintf(pFile, "[%02d/%02d/%02d %02d:%02d:%02d] %s(%d)\n", currentTm->tm_year + 1900, currentTm->tm_mon + 1, currentTm->tm_mday, currentTm->tm_hour, currentTm->tm_min, currentTm->tm_sec, pLog, argv);
        }
        else
        {
            fprintf(pFile, "[%02d/%02d/%02d %02d:%02d:%02d] %s\n", currentTm->tm_year + 1900, currentTm->tm_mon + 1, currentTm->tm_mday, currentTm->tm_hour, currentTm->tm_min, currentTm->tm_sec, pLog);
        }
        fclose(pFile);
    }
}
