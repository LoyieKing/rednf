#ifndef MEMORYCASHMANAGER_H_
#define MEMORYCASHMANAGER_H_

#include <map>
#include <queue>
#include <deque>
#include <list>
#include <algorithm>
#include <string>
#include <vector>
#include "Thread.h"

#include "PacketHeader.h"

class CApplication;
class CCashObject;
class CMemoryCashManager;
class CUser;

// from GuildDomain.h
class CMemoryCashManager
{
public:
    CMemoryCashManager();
    ~CMemoryCashManager();
    void Init(CApplication* app);
    bool IsRightObject(CUser* user);
    int InsertCashMemorySetCharacterObject(CUser* user);
    bool SetUserObject(CUser* user);
    void ProcessLifeTimeOut();
    void DeleteCashObjecct(unsigned int dbid);
    bool QueryCashMemoryBlackList(CUser* user);
    std::map<unsigned int, CCashObject*> m_cashObjects;  // +0
    CApplication* m_app;                                 // +0x18
};

#endif
