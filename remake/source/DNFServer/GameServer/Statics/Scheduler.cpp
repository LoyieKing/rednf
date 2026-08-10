// df_statics_r — Scheduler（ORIG Scheduler.o 拆分）
#include <time.h>

#include "Scheduler.h"

CScheduler::CScheduler()
{
    m_sec = 0xff;
    m_min = 0xff;
    m_hour = 0xff;
    m_day = 0xff;
    m_week = 0xffff;
    m_flag1 = 0xff;
    m_flag2 = 0xff;
}
CScheduler::~CScheduler()
{
}
void CScheduler::SetSpecialHour(int hour)
{
    m_hour = (char)hour;
    m_min = 0;
}
int CScheduler::IsOnTimeSpecialHour(int hour, int min)
{
    if ((char)m_hour == hour && (char)m_min == min)
    {
        return 1;
    }
    return 0;
}
void CScheduler::SetSpecialDayHour(int day, int hour)
{
    m_hour = (char)hour;
    m_day = (char)day;
    m_min = 0;
}
int CScheduler::IsOnTimeSpecialDayHour(int day, int hour, int min)
{
    if ((char)m_day == day && (char)m_hour == hour && (char)m_min == min)
    {
        return 1;
    }
    return 0;
}
bool CheckDailyScheduleTimeOver(int hour, long t)
{
    time_t now;
    time(&now);
    tm local = *localtime(&now);
    tm local2 = local;
    local2.tm_hour = hour;
    local2.tm_min = 0;
    local2.tm_sec = 0;
    long lt = mktime(&local2);
    if (local.tm_hour < hour)
    {
        lt -= 0x15180;
    }
    return t < lt;
}
bool CheckDayHourScheduleTimeOver(int day, int hour, long t)
{
    time_t now;
    time(&now);
    tm local = *localtime(&now);
    tm local2 = local;
    local2.tm_hour = hour;
    local2.tm_min = 0;
    local2.tm_sec = 0;
    long lt = mktime(&local2);
    if (local.tm_hour < hour)
    {
        lt -= 0x15180;
    }
    return t < lt + (1 - day) * 0x15180;
}
