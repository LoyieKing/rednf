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
    // ORIG _ZN10TimerEntryC1Ev 0x847e85c：objType=0 / param1=0 / msg=0x75(DEFAULT) /
    // time=0 / param2=0 / param3=0，逐字段 movl 清零。
    objType = OBJ_TYPE_0;
    param1 = 0;
    msg = TIMER_MESSAGE_DEFAULT;
    time = 0;
    param2 = 0;
    param3 = 0;
}
bool TimerEntry::operator<(const TimerEntry& other) const throw()
{
    // b28d-49 @0x80f6364：ORIG 24 条 —— this.lo→%ecx、this.hi→%ebx（callee-saved，
    // push esi/ebx），hi 有符号 jg/jl、lo 无符号 jae，结果经 %esi 归一返回。
    // 源码形 = 纯 register 存储类（P231，非 asm 绑定）+ 头内 32 位双字视图；
    // 残差（%ecx/%esi 角色互换、比较方向反演）属 4.1 预载操作数 vs 4.4 惰性装载，
    // N7/N18a 封顶（判词见函数头）。
    // b28d-49 判词：N 封顶（N7+N18a）。ORIG 预载 this.lo→%ecx 跨两次 hi 比较、
    // 结果单变量 %esi（`mov $0x1,%esi` … `mov %esi,%eax`）；4.4 恒惰性装载且
    // 只用 callee-saved（探针 h1-h4 四形：%esi/%ebx/%edi 角色互换 + 比较方向反演）。
    // 语义已对齐（hi 有符号、lo 无符号；旧 long long 版 hi 无符号 = 真语义偏差，已修）。
    register unsigned int thisLo = words.lo;
    register int thisHi = words.hi;
    if (thisHi > other.words.hi)
        return true;
    if (thisHi < other.words.hi)
        return false;
    return thisLo >= other.words.lo;
}

TimerQueue::TimerQueue()   // 声明同步去 throw()：ORIG create landing pad 证据
{
    m_map.clear();
}

TimerQueue::~TimerQueue() throw()
{
}

void TimerQueue::InsertTimer(TimerEntry& entry) throw()
{
    // b28d-49 判词：NEAR 封顶（N82 槽布局：Guard/msg/it/end 深槽 vs 我方浅槽 +
    // P61/P178 make_pair 引用传实参）。@0x8630cec
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
    // b28d-49 判词：N 封顶（N82 槽布局整体平移 0x18：ORIG log/it/end/buf 在
    // -0x30..-0x4c，我方 -0x18..-0x34；+ N89 string/迭代器内联）。P167 CLEAR。@0x863101e
    std::string log;
    log.clear();

    std::map<int, int>::iterator it = m_map.begin();
    std::map<int, int>::iterator end = m_map.end();
    for (; it != end; it++)
    {
        char buf[20] = { '\0' };
        sprintf(buf, "(%d %d) ", it->first, it->second);
        log += buf;

        // P23：ORIG 仅当 log 长度 > 0x800 才构造 cMyTrace 并输出。
        if (log.length() > 0x800)
        {
            const char* str = log.c_str();
            cMyTrace trace(__PRETTY_FUNCTION__, 0x101, 0);
            trace("TIMER_QUEUE_LOG : %s", str);
        }
        log.clear();
    }

    if (log.length() != 0)
    {
        const char* str2 = log.c_str();
        cMyTrace trace2(__PRETTY_FUNCTION__, 0x107, 0);
        trace2("TIMER_QUEUE_LOG : %s", str2);
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
