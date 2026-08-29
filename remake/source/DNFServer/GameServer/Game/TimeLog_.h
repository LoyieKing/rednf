// HdrCreate3: ORIG exported symbols, first .h declaration. Evidence: nm ORIG +
// docs/class_func_reports/TimeLog_/setTime.md
// TimeLog_：ORIG T setTime() @0x0854e228 —— time()+localtime_r 后依次写
// +0x00 short（tm_year+0x76c，即完整年份）、+0x02 月+1、+0x03 日、
// +0x04 时、+0x05 分、+0x06 秒。布局按报告汇编写出。
// 2026-08-28：m_mon..m_sec 由 unsigned char 改 signed char——ORIG
// logConsole(0x854f45b)/logToFile(0x854ee79) 以 movsbl 读字段（signed 实证）。
#ifndef DNF_GAME_TIMELOG__H_
#define DNF_GAME_TIMELOG__H_

class TimeLog_
{
public:
    void setTime();   // ORIG T @0x0854e228

    short m_year;          // +0x00（完整年份，tm_year+1900）
    char m_mon;            // +0x02（tm_mon+1）
    char m_day;            // +0x03
    char m_hour;           // +0x04
    char m_min;            // +0x05
    char m_sec;            // +0x06
};  // 报告仅证实 +0x00..+0x06 写入，整体 sizeof 未证实不加断言

#endif  // DNF_GAME_TIMELOG__H_
