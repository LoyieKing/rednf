#ifndef SCHEDULER_H_
#define SCHEDULER_H_

// ---- CScheduler：0x8 ----
class CScheduler
{
public:
    CScheduler();
    ~CScheduler();
    void SetSpecialHour(int hour);
    void SetSpecialDayHour(int day, int hour);
    int IsOnTimeSpecialHour(int hour, int min);
    int IsOnTimeSpecialDayHour(int day, int hour, int min);
    char m_sec;      // +0（ORIG：day 在 +3，+0 未使用）
    char m_min;      // +1
    char m_hour;     // +2
    char m_day;      // +3
    unsigned short m_week;  // +4
    char m_flag1;    // +6
    char m_flag2;    // +7
};

bool CheckDailyScheduleTimeOver(int hour, long t);
bool CheckDayHourScheduleTimeOver(int day, int hour, long t);

#endif // SCHEDULER_H_
