#ifndef WEBEVENT_H_
#define WEBEVENT_H_

#include <map>
#include <queue>
#include <deque>
#include <list>
#include <algorithm>
#include <string>
#include <vector>
#include "Thread.h"

#include "PacketHeader.h"

class CEvent;

// from GuildDomain.h
class CEvent
{
public:
    CEvent();
    virtual ~CEvent();
    // ORIG 证据：_ZTV6CEvent size=0x18（6 槽）⇒ 槽 2/3 为纯虚；ORIG nm 无
    // _ZN6CEvent8setEventEv / _ZN6CEvent10resetEventEv（实现只在派生类），且
    // CPowerWar 槽序为 setEvent→resetEvent（ORIG 0x080a6dee / 0x080a6e54）。
    virtual void setEvent() = 0;     // 槽 2
    virtual void resetEvent() = 0;   // 槽 3
    bool m_isEventOn;        // +4（setEvent=1/resetEvent=0，IsPowerWarOn 判断）
    char m_pad[3];
};

#endif
