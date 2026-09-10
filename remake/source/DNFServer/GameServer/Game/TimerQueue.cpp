// df_game_r TimerQueue 实现（ORIG ctor 0x80f63ac / dtor 0x82a42b8 /
// InsertTimer 0x8630cec / GetTimerMess 0x8630ecc，AE 口径还原）。
#include "TimerQueue.h"
#include "CEnvironment.h"

#include <stdio.h>
#include <iostream>   // std::ios_base::Init __ioinit（ORIG GI 键名 _ZN10TimerQueue11InsertTimerER10TimerEntry 的前提，P338）
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
// [P327 GI 键名] ORIG 本 TU `_GLOBAL__I_` 键名 = _ZN10TimerQueue11InsertTimerER10TimerEntry
// ⇒ 该重载定义须为本 TU 首个强符号（此前 TimerEntry::TimerEntry 定义在前 ⇒ 键名错为
// _GLOBAL__I__ZN10TimerEntryC2Ev）。纯前置函数定义，符号集合/构造序零变化。
void TimerQueue::InsertTimer(TimerEntry& entry) throw()
{
    // b28d-solo-TimerQueue 判词：AE（94=94 逐条，P167 CLEAR）。两处关键：
    // ① `int msg;` 声明前置到 Guard 之前、赋值留在 push 之后 —— 4.4 栈槽按声明序
    //    「先声明浅」（P212/N98），据此把 ORIG 的 Guard(-0x34 深)/msg(-0x30 浅) 翻正；
    // ② ORIG 的 -0x10/-0xc 两槽不是具名 one/msg2，而是 `std::make_pair((int)entry.msg, 1)`
    //    右值引用参数的临时物化 —— 具名左值版选错 make_pair<int&,int&> 实例（ORIG 为
    //    make_pair<int,int>），直接实参形调用点差集归零且逐条 AE（P167 判据②）。
    // 残余具名 mp/cp 形（ndiff 38）与 hoist 形（32）均劣。@0x8630cec
    int msg;
    Guard<Mutex> guard(&m_mutex);
    m_queue.push(entry);

    msg = entry.msg;
    std::map<int, int>::iterator it = m_map.find(msg);
    if (it != m_map.end())
    {
        it->second += 1;
    }
    else
    {
        m_map.insert(std::make_pair((int)entry.msg, 1));
    }
}

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
    // b28d-49 @0x80f6364：ORIG 23 条 —— this.lo→%ecx、this.hi→%ebx（callee-saved，
    // push esi/ebx），hi 有符号 jg/jl、lo 无符号 jae，结果经 %esi 归一返回。
    // 源码形 = 纯 register 存储类（P231，非 asm 绑定）+ 头内 32 位双字视图；
    // 残差（%ecx/%esi 角色互换、比较方向反演）属 4.1 预载操作数 vs 4.4 惰性装载，
    // N7/N18a 封顶（判词见函数头）。
    // b28d-49 判词：N 封顶（N7+N18a）。ORIG 预载 this.lo→%ecx 跨两次 hi 比较、
    // 结果单变量 %esi（`mov $0x1,%esi` … `mov %esi,%eax`）；4.4 恒惰性装载且
    // 只用 callee-saved（探针 h1-h4 四形：%esi/%ebx/%edi 角色互换 + 比较方向反演）。
    // 语义已对齐（hi 有符号、lo 无符号；旧 long long 版 hi 无符号 = 真语义偏差，已修）。
    // b28d-solo-TimerQueue 判词：N 封顶（N7+N18a）。本轮七形实测（fndiff ndiff，
    // ORIG 23 条）：本形(==优先) 24（最小）/ 原 if-return 27 / 三目链 32 /
    // register bool ret 30 / 3×register 32 / 取反尾 27 / 交换 if 序 27 —— 全形非 0。
    // ORIG「other 一次装载(hi→%edx,lo→%eax) + this.lo→%ecx + 结果 %esi 预置
    // + jg/jl/jae 单变量归一」为 4.1 特有寄存器分配；P248 三档旋钮不适用
    //（无字段可换型、返回 bool 已同型）⇒ 语义已对齐，封顶。
    register unsigned int thisLo = words.lo;
    register int thisHi = words.hi;
    if (thisHi == other.words.hi)
        return thisLo >= other.words.lo;
    return thisHi > other.words.hi;
}

TimerQueue::TimerQueue()   // 声明同步去 throw()：ORIG create landing pad 证据
{
    m_map.clear();
}

TimerQueue::~TimerQueue() throw()
{
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
    // b28d-solo-TimerQueue 判词：AE（130=130，P227 全表达式临时链）。原形「具名
    // const char* str + 具名 cMyTrace trace」使 4.4 把 c_str 结果落槽（-0x10/-0xc）
    // 并把 trace 临时压深（-0x44/-0x54）——即 b28d-49「N82 整体浅移 0x18」判词的根因；
    // 改 cMyTrace(...)(...) 后 4.4 在 ctor 前求值 c_str 并以 %ebx 跨 ctor 保活、
    // trace 临时落 -0x2c/-0x18，与 ORIG 逐条 AE。@0x863101e
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
            cMyTrace(__PRETTY_FUNCTION__, 0x101, 0)("TIMER_QUEUE_LOG : %s", log.c_str());
        }
        log.clear();
    }

    if (log.length() != 0)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x107, 0)("TIMER_QUEUE_LOG : %s", log.c_str());
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
