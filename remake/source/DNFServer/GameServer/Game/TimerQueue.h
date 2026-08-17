#ifndef GAME_TIMERQUEUE_H_
#define GAME_TIMERQUEUE_H_

#include <map>
#include <queue>
#include <vector>

#include "GameBase.h"

// 定时器消息号（TimerDispatcher 数组下标，值取自 ORIG 调用点；
// 枚举名/完整取值待 TimerSig/TimerDispatcher 组确认）。
#ifndef DNF_ENUM_TIMER_MESSAGE_DEFINED
#define DNF_ENUM_TIMER_MESSAGE_DEFINED
enum TIMER_MESSAGE
{
    TIMER_MESSAGE_DEFAULT = 0x75,
    TIMER_MESSAGE_0x62 = 0x62,
    TIMER_MESSAGE_0x96 = 0x96,
    TIMER_MESSAGE_0xa3 = 0xa3
};
#endif

// 28 字节：objType/param1/msg + long long time + param2/param3
struct TimerEntry
{
    enum OBJ_TYPE
    {
        OBJ_TYPE_0 = 0,
        OBJ_TYPE_2 = 2,
        OBJ_TYPE_7 = 7
    };

    // ORIG：ctor 与 operator< 为 throw() 声明 —— 调用方（InsertTimerInMilisecond
    // 0x8630e60）无 EH 清理块；析构为隐式平凡（局部 TimerEntry 作用域退出无
    // _ZN10TimerEntryD1Ev 调用，且 STL 堆函数按值传参 0x14(%ebp) 实证）。
    TimerEntry() throw();
    bool operator<(const TimerEntry& other) const throw();

    int objType;          // +0x00
    int param1;           // +0x04
    TIMER_MESSAGE msg;    // +0x08
    long long time;       // +0x0c
    int param2;           // +0x14
    unsigned int param3;  // +0x18
};

// df_game_r TimerQueue（ORIG 0x80f63ac / 0x8630cec）。
// 布局：priority_queue @ +0x00（0x10），Mutex @ +0x10（0x18），map @ +0x28。
class TimerQueue
{
public:
    // ORIG 类方法均 throw()（调用方无 EH 清理实证，见 InsertTimerInMilisecond
    // 0x8630e60 与 DispatchThread::dispatch 0x847e2d2）。
    TimerQueue() throw();
    ~TimerQueue() throw();

    void InsertTimer(TimerEntry& entry) throw();
    void InsertTimer(TimerEntry::OBJ_TYPE objType, int param1,
                     TIMER_MESSAGE msg, int sec, int param2, unsigned int param3) throw();
    void InsertTimerInMilisecond(TimerEntry::OBJ_TYPE objType, int param1,
                                 TIMER_MESSAGE msg, long long milisecond,
                                 int param2, unsigned int param3) throw();
    bool GetTimerMess(TimerEntry& entry) throw();
    size_t GetQueueSize() throw();
    void write_timer_queue_log() throw();

private:
    std::priority_queue<TimerEntry, std::vector<TimerEntry>,
                        std::less<TimerEntry> > m_queue;   // +0x00
    Mutex m_mutex;                                         // +0x10
    std::map<int, int> m_map;                              // +0x28
};

#endif  // GAME_TIMERQUEUE_H_
