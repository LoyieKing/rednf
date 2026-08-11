// df_dbmw_r - CMisc (ORIG CMisc.cpp)
#include "DBMWCommon.h"

#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <algorithm>
#include <unistd.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <sys/resource.h>
#include <signal.h>
#include <fcntl.h>
#include <stdarg.h>
#include <sys/stat.h>
#include <sys/times.h>

#include "DNFFileLog.h"
#include "DNFFunctionLib.h"

int getErrno();

// ---- packet 构造（出库化）----
int get_day_interval(struct tm* a, struct tm* b);

int get_awardItem_using_interval()
{
    time_t t = time(0);
    struct tm* now = localtime(&t);
    struct tm base;
    memset(&base, 0, 0x2c);
    base.tm_year = 106;
    base.tm_mon = 6;
    base.tm_mday = 1;
    now->tm_mon += 1;
    if (now->tm_mon > 0xb)
    {
        now->tm_mon = 0;
        now->tm_year += 1;
    }
    now->tm_hour = 0;
    now->tm_min = 0;
    now->tm_sec = 0;
    return get_day_interval(&base, now);
}
void get_awardItem_using_interval(int monthInterval)
{
    time_t t = time(0);
    struct tm* now = localtime(&t);
    struct tm base;
    memset(&base, 0, 0x2c);
    base.tm_year = 106;
    base.tm_mon = 6;
    base.tm_mday = 1;
    int months = monthInterval % 0xc;
    int years = monthInterval / 0xc;
    if (0 < years)
        now->tm_year += years;
    now->tm_mon += months;
    if (0xb < now->tm_mon)
    {
        now->tm_mon -= 0xb;
        now->tm_year += 1;
    }
    now->tm_hour = 0;
    now->tm_min = 0;
    now->tm_sec = 0;
    get_day_interval(&base, now);
}
int get_day_interval(struct tm* a, struct tm* b)
{
    time_t ta = mktime(a);
    time_t tb = mktime(b);
    return (int)((tb - ta) / 86400);
}
bool isDayTimeOver(unsigned int timestamp, unsigned int days)
{
    struct tm t;
    time_t now;
    time(&now);
    t = *localtime(&now);
    t.tm_mday -= days;
    time_t limit = mktime(&t);
    return (int)timestamp < (int)limit;
}
void* operator new(std::size_t size, const std::nothrow_t&) throw()
{
    void* p = 0;
    try
    {
        p = ::operator new(size);
    }
    catch (...)
    {
    }
    return p;
}
void operator delete(void* ptr, const std::nothrow_t&) throw()
{
    if (ptr)
        free(ptr);
}
int getErrno()
{
    return *__errno_location();
}
int parse_string(std::vector<std::string>& v, std::string& s, char c)
{
    int len = s.size();
    int pos = 0;
    std::string tok;
    while (pos < len)
    {
        int found = s.find(c, pos);
        if (found == (int)std::string::npos)
        {
            tok = s.substr(pos, len - pos);
            if (!tok.empty())
                v.push_back(tok);
            break;
        }
        tok = s.substr(pos, found - pos);
        if (!tok.empty())
            v.push_back(tok);
        pos = found + 1;
    }
    return 1;
}
unsigned long long TIME_to_ulonglong_date(void* t)
{
    unsigned int* p = (unsigned int*)t;
    return (unsigned long long)(p[0] * 10000 + p[1] * 100 + p[2]);
}
unsigned long long TIME_to_ulonglong_time(void* t)
{
    unsigned int* p = (unsigned int*)t;
    return (unsigned long long)(p[3] * 10000 + p[4] * 100 + p[5]);
}
unsigned long long TIME_to_ulonglong(void* t)
{
    return 0;
}
unsigned long long TIME_to_ulonglong_datetime(void* t)
{
    return 0;
}
