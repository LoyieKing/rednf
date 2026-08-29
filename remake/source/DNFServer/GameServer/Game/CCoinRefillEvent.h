#ifndef GAME_CCOINREFILLEVENT_H_
#define GAME_CCOINREFILLEVENT_H_

#include <time.h>

#include "CGuildServerProxy.h"  // CEventBase 基类 / Word_Param（StartEvent(Word_Param) 按值）

// CCoinRefillEvent —— 金币补充 repeat 事件（CEventManager 事件 idx 0xd，
// 由 TimerEventCoinRefill::dispatch_sig 0x086364c4 经 GetRepeatEvent(0xd) 取得）。
// 布局证据：ctor ORIG 0x0832bdf4（CEventBase 基区构造后 memset(this+0xc,-1,0x14)）、
// StartEvent(Word_Param) 0x0832be82（+0xc 计数、+0x10/+0x14/+0x18 小时槽）、
// getNextEventTime 0x0832c148（索引 i∈[1..count] → this+i*4+0xc，兜底 *(this+0x10)）。
class CCoinRefillEvent : public CEventBase
{
public:
    CCoinRefillEvent();                              // 0x0832bdf4
    virtual ~CCoinRefillEvent();                     // D1/D2 0x0832be2e / 0x0832be5e
    virtual void StartEvent();                       // 0x0832be7c（纯 ret）
    virtual void StartEvent(Word_Param param);       // 0x0832be82
    virtual void EndEvent();                         // 0x0832c048
    int getLastEventTime() const;                    // 0x0832c0b8
    int getNextEventTime(tm const& cur) const;       // 0x0832c148
    void registNextRefillTimer() const;              // 0x0832c1fe

private:
    int m_timeCount;   // +0x0c（已登记小时槽数，0..4；-1 = 事件未初始化）
    int m_times[4];    // +0x10..+0x1c（小时槽；m_times[0] 为当前小时基准槽，
                       //        m_times[i-1] 对应逻辑槽 i，即 ORIG this+i*4+0xc）
};

#endif  // GAME_CCOINREFILLEVENT_H_
