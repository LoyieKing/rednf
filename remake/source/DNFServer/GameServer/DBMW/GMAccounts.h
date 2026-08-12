#ifndef DBMW_GMACCOUNTS_H_
#define DBMW_GMACCOUNTS_H_

#include "DBMWCommon.h"


namespace WongWork
{
class CGMAccounts
{
public:
    struct stGMInfo_t
    {
        bool operator==(const stGMInfo_t& other) const;
        int m_field0;  // +0
        int m_field1;  // +4
    };
    CGMAccounts();
    ~CGMAccounts();
    int loadGMAccounts(char const* path);
    bool isGM(unsigned int id);
    stGMInfo_t getGMInfo(unsigned int id) const;
    int appendGM(unsigned int id, unsigned int flag);
    int removeGM(unsigned int id, unsigned int flag);
    void clearGmList();
    void LoadGmList(unsigned int idx, int flag);
    void AppendGM_Sys(unsigned int id, char flag);
    std::list<stGMInfo_t> m_list;  // +0
};
}


#endif  // DBMW_GMACCOUNTS_H_
