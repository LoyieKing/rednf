#ifndef SCHEDULER_H_
#define SCHEDULER_H_

#include <map>
#include <queue>
#include <deque>
#include <list>
#include <algorithm>
#include <string>
#include <vector>
#include "Thread.h"

#include "PacketHeader.h"
#include "Scheduler.h"

class CScheduler;
struct STPowerWarScheduleTime;

// from GuildDomain.h
struct STPowerWarScheduleTime
{
    char m_data[3];   // {type@0, hour@1, min@2}（ORIG：POD 3 字节，vector 元素步进 3）
};

// from GuildDomain.h
class CScheduler
{
public:
    CScheduler();
    ~CScheduler();
    void SetSpecialHour(int hour);
    void SetSpecialDayHour(int day, int hour);
    void SetSpecificDayScheduleHour(int day, int hour);
    int IsOnTimeSpecialHour(int hour, int min);
    int IsOnTimeSpecialDayHour(int day, int hour, int min);
    int GetSpecificDayScheduleHour(int day);
    void Clear();
    void SetSpecialWeekDayHour(std::vector<STPowerWarScheduleTime> schedule);
    void SetSpecialWeekDayHour(int day, int hour);
    int IsOnTimeSpecialWeekDayHour(int day, int hour, int min);
    STPowerWarScheduleTime* GetNextScheduleTime(unsigned char& hour, unsigned char& min);
    char m_day;       // +0
    char m_min;       // +1
    char m_hour;      // +2
    char m_sec;       // +3
    unsigned short m_week;  // +4
    char m_flag1;     // +6
    char m_flag2;     // +7
    char m_table[28]; // +8（周时刻表：7×4）
};

int CheckDayScheduleTimeOver(int hour, long t);
bool CheckDailyScheduleTimeOver(int hour, long t);
int CheckDayHourScheduleTimeOver(int day, int hour, long t);
void GetScheduleTimeAsWDay(int day, int hour);

#endif
