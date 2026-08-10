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
template <> Mutex GlobalInstance<ScriptData>::m_lock = Mutex();

template <> ScriptData* GlobalInstance<ScriptData>::create()
{
    if (m_p == 0)
    {
        m_lock.lock();
        if (m_p == 0)
        {
            m_p = new ScriptData;
        }
        m_lock.unlock();
    }
    return m_p;
}

template <> ScriptData* GlobalInstance<ScriptData>::inst_ptr()
{
    create();
    return m_p;
}

template <> Script* GlobalInstance<Script>::m_p = 0;
template <> Mutex GlobalInstance<Script>::m_lock = Mutex();

template <> Script* GlobalInstance<Script>::create()
{
    if (m_p == 0)
    {
        m_lock.lock();
        if (m_p == 0)
        {
            m_p = new Script;
        }
        m_lock.unlock();
    }
    return m_p;
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

static __thread char g_num_buf[0x400];

char* NumberToString(unsigned int value, int index)
{
    sprintf(g_num_buf + index * 0x40, "%u", value);
    return g_num_buf + index * 0x40;
}

char* NumberToString(unsigned long long value, int index)
{
    sprintf(g_num_buf + index * 0x40, "%qu", value);
    return g_num_buf + index * 0x40;
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
    long long now = get_ms_tick();
    time_t t = now / 1000;
    struct tm* tm_now = localtime(&t);
    char filename[256] = {0};
    snprintf(filename, 0x100, "./log/Relay%4d%02d%02d_T%d.log",
             tm_now->tm_year + 1900, tm_now->tm_mon + 1, tm_now->tm_mday,
             *(unsigned short*)((char*)G_ScriptData() + 4));
    FILE* f = fopen(filename, "a+");
    if (f != 0)
    {
        fprintf(f, "[%02d/%02d/%02d %02d:%02d:%02d] Error: %s\n",
                tm_now->tm_year + 1900, tm_now->tm_mon + 1, tm_now->tm_mday,
                tm_now->tm_hour, tm_now->tm_min, tm_now->tm_sec, msg);
        fclose(f);
    }
}

static unsigned int g_prev_acc_id = 0;

void MonitorAuthLog(unsigned int acc_id)
{
    if (g_prev_acc_id != acc_id)
    {
        g_prev_acc_id = acc_id;
        long long now = get_ms_tick();
        time_t t = now / 1000;
        struct tm* tm_now = localtime(&t);
        char filename[256] = {0};
        snprintf(filename, 0x100, "./log/Relay%4d%02d%02d_T%d.log",
                 tm_now->tm_year + 1900, tm_now->tm_mon + 1, tm_now->tm_mday,
                 *(unsigned short*)((char*)G_ScriptData() + 4));
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
}
