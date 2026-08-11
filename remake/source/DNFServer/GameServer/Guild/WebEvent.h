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
    unsigned char m_field4;  // +4
    char m_pad[3];
};

#endif
