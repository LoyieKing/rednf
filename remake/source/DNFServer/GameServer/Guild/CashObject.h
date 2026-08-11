#ifndef CASHOBJECT_H_
#define CASHOBJECT_H_

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
class CCashObject;

// from GuildDomain.h
class CCashObject
{
public:
    CCashObject();
    ~CCashObject();
    static void* operator new(unsigned int size);
    static void operator delete(void* p);
    static void operator delete(void* p, unsigned int size);
    bool IsLifeTimeOut();
    void SetBlackUsersObject(std::map<unsigned int, CBlackUser*>& blackUsers);
    void ClearBlackUsers();
    std::map<unsigned int, CBlackUser*>* GetBlackUsersObject();
    void SetCharacNo(unsigned int charNo);
    int m_lifeTime;      // +0
    unsigned int m_charNo;  // +4
    std::map<unsigned int, CBlackUser*> m_blackUsers;  // +8
    char m_pad[4];       // +0x20
};

#endif
