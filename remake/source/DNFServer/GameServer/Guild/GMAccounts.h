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
        unsigned int m_id;    // +0（== 比较；LoadGmList 为 group，AppendGM_Sys 为 id）
        unsigned int m_flag;  // +4（LoadGmList 为 index，AppendGM_Sys 为 flag）
    };
    void LoadGmList(unsigned int group, int index);
    void clearGmList();
    void AppendGM_Sys(unsigned int id, char flag);
    int loadGMAccounts(const char* path);
    int isGM(unsigned int id);
    int appendGM(unsigned int id, unsigned int value);
    int removeGM(unsigned int id, unsigned int value);
    stGMInfo_t getGMInfo(unsigned int id) const;
    std::list<stGMInfo_t> m_list;
};
}

#endif
