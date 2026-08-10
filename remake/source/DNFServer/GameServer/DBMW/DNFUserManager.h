#ifndef DBMW_DNFUSERMANAGER_H_
#define DBMW_DNFUSERMANAGER_H_

#include "DBMWCommon.h"


class CUserManager
{
public:
    CUserManager();
    ~CUserManager();
    void Init(CApplication* app);
    char InsertProhibitUser(unsigned int dbid, CDNFProhibitUser* pu);
    CDNFProhibitUser* FindProhibitUser(unsigned int dbid) const;
    char DeleteProhibitUser(unsigned int dbid);
    void ProcessByMinute();
    std::map<unsigned int, CDNFProhibitUser*> m_prohibitUsers;  // +0
    CApplication* m_app;  // +0x18
};

#endif  // DBMW_DNFUSERMANAGER_H_
