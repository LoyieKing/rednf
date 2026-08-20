// df_game_r Game/ 工具函数实现（独立 TU，从 GameStubs.cpp 拆出）。
// 包含：trim / less_TCHAR / TimerEntry / SECOND_TO_MILISECOND / cMyTrace。
#include "CEnvironment.h"
#include "TimerQueue.h"
#include "GameBase.h"
#include "GlobalData.h"
#include <cstring>
#include <sys/time.h>
#include <sys/select.h>
#include <time.h>

// ---- trim（CEnvironment::parse 用，ORIG 全局函数）----
void trim(char* s)
{
    if (!s) return;
    char* end = s + strlen(s);
    while (end > s && (end[-1] == ' ' || end[-1] == '\t' || end[-1] == '\r' || end[-1] == '\n'))
        *--end = 0;
    char* p = s;
    while (*p == ' ' || *p == '\t' || *p == '\r' || *p == '\n') ++p;
    if (p != s) memmove(s, p, strlen(p) + 1);
}

// ---- less_TCHAR（CEnvironment map 比较器）----
bool less_TCHAR::operator()(const char* const& x, const char* const& y) const
{
    return strcmp(x, y) < 0;
}

// ---- SECOND_TO_MILISECOND（GameBase.h 声明）----
long long SECOND_TO_MILISECOND(int sec)
{
    return (long long)sec * 1000;
}

// ---- cMyTrace（GameTypes.h 声明，ORIG 0x854f746 族）----
cMyTrace::cMyTrace(const char*, int, int) {}
cMyTrace::cMyTrace(const char*, int, int, bool bFieldC, bool bFieldD)
{
    m_field_c = (char)bFieldC;
    m_field_d = (char)bFieldD;
}
void cMyTrace::operator()(const char*, ...) {}

// ---- CheckDailyScheduleTime（每日调度时间检查，ORIG _Z22CheckDailyScheduleTimeill）----
bool CheckDailyScheduleTime(int nScheduleTime, long lLastPlayTime, long lCurTime)
{
    if (nScheduleTime <= 0)
        return false;
    long lastDay = lLastPlayTime / nScheduleTime;
    long curDay = lCurTime / nScheduleTime;
    return curDay > lastDay;
}

// ============================================================================
// 基础时间工具（ORIG-only 符号，被多个 TU 依赖；逐函数核对 ORIG 反汇编）
// ============================================================================

// ---- sleep_ext（ORIG _Z9sleep_extii，0x86b1b48，0x3f 字节）----
// ORIG：构造 {sec, usec} timeval 后 select(0,0,0,0,&tv)（sleep 的 select 实现）。
void sleep_ext(int sec, int usec)
{
    struct timeval tv;
    tv.tv_sec = sec;
    tv.tv_usec = usec;
    select(0, NULL, NULL, NULL, &tv);
}

// ---- getPrevScheduleTime（ORIG _Z19getPrevScheduleTimeil，0x846c2e9，0x61 字节）----
// ORIG：t = time - hour*3600；localtime_r；置 hour:00:00；mktime。
// 返回 time 当日 hour 时刻的时间戳（未到 hour 则属前一天）。
long getPrevScheduleTime(int hour, long time)
{
    long t = time - (long)hour * 3600;
    struct tm tmv;
    localtime_r(&t, &tmv);
    tmv.tm_hour = hour;
    tmv.tm_min = 0;
    tmv.tm_sec = 0;
    return (long)mktime(&tmv);
}

// ---- getDailyInitializeTime（ORIG _Z22getDailyInitializeTimeiii，0x86b1f5a，0x66 字节）----
// ORIG：取 CSystemTime::getCurSec()，localtime_r，置 hour:min:sec，mktime；
// 若结果晚于当前时刻则回退一天（返回当日该时刻，已过则昨日）。
int getDailyInitializeTime(int hour, int min, int sec)
{
    time_t cur = GlobalData::s_systemTime_.getCurSec();
    struct tm tmv;
    localtime_r(&cur, &tmv);
    tmv.tm_hour = hour;
    tmv.tm_min = min;
    tmv.tm_sec = sec;
    time_t ret = mktime(&tmv);
    if (ret > cur)
        ret -= 86400;
    return (int)ret;
}

// ---- __GetCurrentDNFDate（ORIG _Z19__GetCurrentDNFDateR2tm，0x86b1fc0，0x5f 字节）----
// ORIG：cur = getCurSec()；若 localtime 后 tm_hour < CEnvironment+0x37c
// （m_serverEnvir.m_fatigueTime，即每日调度小时）则回退一天再 localtime。
void __GetCurrentDNFDate(tm& out)
{
    time_t cur = GlobalData::s_systemTime_.getCurSec();
    int daily = G_CEnvironment()->m_serverEnvir.m_fatigueTime;  // +0x37c
    localtime_r(&cur, &out);
    if (out.tm_hour < daily)
    {
        cur -= 86400;
        localtime_r(&cur, &out);
    }
}

// ---- ComputeDaysScheduleTime（ORIG _Z23ComputeDaysScheduleTimeill，0x846c144，0x1a5 字节）----
// ORIG：分别以 curTime/lastTime 求当日 hour:00:00 基准时刻（未到 hour 回退一天），
// 差值 /86400 取整天数，负数归 0。
int ComputeDaysScheduleTime(int hour, long lastTime, long curTime)
{
    time_t c = curTime;
    struct tm tmc;
    localtime_r(&c, &tmc);
    struct tm t1 = tmc;
    t1.tm_hour = hour;
    t1.tm_min = 0;
    t1.tm_sec = 0;
    time_t r1 = mktime(&t1);
    if (tmc.tm_hour < hour)
        r1 -= 86400;

    time_t l = lastTime;
    struct tm tml;
    localtime_r(&l, &tml);
    struct tm t2 = tml;
    t2.tm_hour = hour;
    t2.tm_min = 0;
    t2.tm_sec = 0;
    time_t r2 = mktime(&t2);
    if (tml.tm_hour < hour)
        r2 -= 86400;

    int days = (int)(r1 - r2) / 86400;
    if (days < 0)
        days = 0;
    return days;
}
