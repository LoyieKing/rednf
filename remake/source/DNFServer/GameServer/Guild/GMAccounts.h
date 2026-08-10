#ifndef GMACCOUNTS_H_
#define GMACCOUNTS_H_

#include <deque>
#include <map>
#include <queue>
#include <string>
#include "Thread.h"
#include <list>
#include <algorithm>
#include <vector>

#include "PacketHeader.h"
#include "GMAccounts.h"

class CGMAccounts;

// from GuildApp.h
namespace WongWork { class CGMAccounts; }

// from GuildDomain.h
namespace WongWork
{
class CGMAccounts
{
public:
    struct stGMInfo_t
    {
        bool operator==(const stGMInfo_t& other) const;
        unsigned int m_field0;
        unsigned char m_field1;
    };
    void LoadGmList(unsigned int group, int index);
    void clearGmList();
    void AppendGM_Sys(unsigned int id, char flag);
    void loadGMAccounts(const char* path);
    int isGM(unsigned int id);
    void appendGM(unsigned int id, unsigned int value);
    void removeGM(unsigned int id, unsigned int value);
    stGMInfo_t getGMInfo(unsigned int id) const;
    std::list<stGMInfo_t> m_list;
};
}

#endif
