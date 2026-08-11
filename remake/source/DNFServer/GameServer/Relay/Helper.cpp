// Relay 基础工具函数（df_relay_r, GCC 4.1.2, 无 DWARF — Ghidra 反汇编还原）
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>

#include "Helper.h"
#include "ScriptData.h"
#include "Script.h"


template <> ScriptData* GlobalInstance<ScriptData>::m_p = 0;
template <> Mutex GlobalInstance<ScriptData>::sync = Mutex();

template <> void GlobalInstance<ScriptData>::create()
{
    if (m_p == 0)
    {
        sync.lock();
        if (m_p == 0)
        {
            m_p = new ScriptData;
        }
        sync.unlock();
    }
    return;
}

template <> ScriptData* GlobalInstance<ScriptData>::inst_ptr()
{
    create();
    return m_p;
}

template <> Script* GlobalInstance<Script>::m_p = 0;
template <> Mutex GlobalInstance<Script>::sync = Mutex();

template <> void GlobalInstance<Script>::create()
{
    if (m_p == 0)
    {
        sync.lock();
        if (m_p == 0)
        {
            m_p = new Script;
        }
        sync.unlock();
    }
    return;
}

template <> Script* GlobalInstance<Script>::inst_ptr()
{
    create();
    return m_p;
}

ScriptData* G_ScriptData()
{
    return GlobalInstance<ScriptData>::inst_ptr();
}

Script* G_Script()
{
    return GlobalInstance<Script>::inst_ptr();
}

__thread char gNumberToStringBuffer[0x200];

char* NumberToString(unsigned int value, int index)
{
    sprintf((char*)((unsigned int)index * 0x40 +
                    (unsigned int)gNumberToStringBuffer), "%u", value);
    return (char*)((unsigned int)index * 0x40 +
                   (unsigned int)gNumberToStringBuffer);
}

char* NumberToString(unsigned long long value, int index)
{
    sprintf((char*)((unsigned int)index * 0x40 +
                    (unsigned int)gNumberToStringBuffer), "%qu", value);
    return (char*)((unsigned int)index * 0x40 +
                   (unsigned int)gNumberToStringBuffer);
}

long long get_ms_tick()
{
    struct timeval tv;
    gettimeofday(&tv, 0);
    return (long long)tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

void make_dir(char* path)
{
    DIR* d = opendir(path);
    if (d == 0)
    {
        mkdir(path, 0x1ff);
    }
    else
    {
        closedir(d);
    }
}

void WriteLog(const char* msg)
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
        fprintf(f, "[%02d/%02d/%02d %02d:%02d:%02d] Error: %s\n",
                tm_now->tm_year + 1900, tm_now->tm_mon + 1, tm_now->tm_mday,
                tm_now->tm_hour, tm_now->tm_min, tm_now->tm_sec, msg);
        fclose(f);
    }
}

void MonitorAuthLog(unsigned int acc_id)
{
    static unsigned int gPrevAccId;
    if (gPrevAccId == acc_id)
    {
        return;
    }
    gPrevAccId = acc_id;
    time_t t = get_ms_tick() / 1000;
    struct tm* tm_now = localtime(&t);
    char filename[256] = {0};
    snprintf(filename, 0x100, "./log/Relay%4d%02d%02d_T%d.log",
             tm_now->tm_year + 1900, tm_now->tm_mon + 1, tm_now->tm_mday,
             G_ScriptData()->mPortTcp);
    FILE* f = fopen(filename, "a+");
    if (f != 0)
    {
        fprintf(f,
                "[%02d/%02d/%02d %02d:%02d:%02d] Monitor Auth: Fail.. Disconnect User!! m_id: %s\n",
                tm_now->tm_year + 1900, tm_now->tm_mon + 1, tm_now->tm_mday,
                tm_now->tm_hour, tm_now->tm_min, tm_now->tm_sec,
                NumberToString(acc_id, 0));
        fclose(f);
    }
}
