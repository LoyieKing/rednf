// df_game_r TimerQueue 实现（ORIG ctor 0x80f63ac / dtor 0x82a42b8 /
// InsertTimer 0x8630cec / GetTimerMess 0x8630ecc，AE 口径还原）。
#include "TimerQueue.h"
#include "CEnvironment.h"

#include <stdio.h>
#include <string>

// 全局时钟对象属于 GlobalData（ORIG BSS 0x941f714），真实定义在其它 TU。
class CSystemTime
{
public:
    unsigned int getCurTickCount();
};

struct GlobalData
{
    static CSystemTime s_systemTime_;
};
// TimerEntry（ORIG 0x8630e60 族调用方无 EH 清理块，throw() 对齐）
TimerEntry::TimerEntry() throw()
{
}
bool TimerEntry::operator<(const TimerEntry&) const throw() { return false; }

TimerQueue::TimerQueue() throw()
{
    m_map.clear();
}

TimerQueue::~TimerQueue() throw()
{
}

void TimerQueue::InsertTimer(TimerEntry& entry) throw()
{
    Guard<Mutex> guard(&m_mutex);
    m_queue.push(entry);

    int msg = entry.msg;
    std::map<int, int>::iterator it = m_map.find(msg);
    if (it != m_map.end())
    {
        it->second += 1;
    }
    else
    {
        int one = 1;
        int msg2 = entry.msg;
        std::pair<int, int> mp = std::make_pair(msg2, one);
        std::pair<const int, int> cp(mp);
        m_map.insert(cp);
    }
}

void TimerQueue::InsertTimer(TimerEntry::OBJ_TYPE objType, int param1,
                             TIMER_MESSAGE msg, int sec, int param2, unsigned int param3) throw()
{
    InsertTimerInMilisecond(objType, param1, msg, SECOND_TO_MILISECOND(sec), param2, param3);
}

void TimerQueue::InsertTimerInMilisecond(TimerEntry::OBJ_TYPE objType, int param1,
                                         TIMER_MESSAGE msg, long long milisecond,
                                         int param2, unsigned int param3) throw()
{
    TimerEntry entry;
    entry.objType = objType;
    entry.param1 = param1;
    entry.msg = msg;
    entry.time = GlobalData::s_systemTime_.getCurTickCount() + milisecond;
    entry.param2 = param2;
    entry.param3 = param3;
    InsertTimer(entry);
}

bool TimerQueue::GetTimerMess(TimerEntry& entry) throw()
{
    unsigned int cur = GlobalData::s_systemTime_.getCurTickCount();

    Guard<Mutex> guard(&m_mutex);
    if (m_queue.empty())
    {
        return false;
    }

    entry = m_queue.top();
    if (entry.time <= cur)
    {
        m_queue.pop();

        std::map<int, int>::iterator it = m_map.find(entry.msg);
        if (it != m_map.end())
        {
            it->second -= 1;
        }
        return true;
    }
    return false;
}

size_t TimerQueue::GetQueueSize() throw()
{
    return m_queue.size();
}

void TimerQueue::write_timer_queue_log() throw()
{
    std::string log;
    log.clear();

    std::map<int, int>::iterator it = m_map.begin();
    std::map<int, int>::iterator end = m_map.end();
    for (; it != end; it++)
    {
        char buf[20] = {0};
        sprintf(buf, "(%d %d) ", it->first, it->second);
        log += buf;

        if (log.length() > 0x800)
        {
            cMyTrace trace("void TimerQueue::write_timer_queue_log()", 0x101, 0);
            trace("TIMER_QUEUE_LOG : %s", log.c_str());
            log.clear();
        }
    }

    if (log.length() != 0)
    {
        cMyTrace trace("void TimerQueue::write_timer_queue_log()", 0x107, 0);
        trace("TIMER_QUEUE_LOG : %s", log.c_str());
    }
}

// ---- 全局单例访问器 ----
// ORIG 0x80f647c W（_Z12G_TimerQueuev）：GlobalInstance<TimerQueue>::inst_ptr()
// （create 惰性 new 并缓存至 m_p，ORIG m_p 位于 BSS 0x93facc4）。
TimerQueue* G_TimerQueue()
{
    return GlobalInstance<TimerQueue>::inst_ptr();
}
template class GlobalInstance<TimerQueue>;
