#ifndef BLACKUSER_H_
#define BLACKUSER_H_

#include <map>
#include <queue>
#include <deque>
#include <list>
#include <algorithm>
#include <string>
#include <vector>
#include "Thread.h"

#include "PacketHeader.h"

class CBlackUser;
struct STBlackUserDBType;

// from GuildDomain.h
struct STBlackUserDBType
{
    STBlackUserDBType();
    int m_field0;       // +0
    char m_name[0x20];  // +4
    int m_occurTime;    // +0x24（总大小 0x28）
};

// from GuildDomain.h
class CBlackUser
{
public:
    CBlackUser();
    static void* operator new(unsigned int size);
    static void operator delete(void* p);
    static void operator delete(void* p, unsigned int size);
    void SetBlackUser(char* name, unsigned int time);
    char* GetName();
    unsigned int GetOccurTime();
    char m_data[0x20];
    unsigned int m_time;  // +0x20
    char m_pad[4];        // +0x24
};

#endif
