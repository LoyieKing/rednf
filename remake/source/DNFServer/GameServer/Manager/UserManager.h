#ifndef USER_MANAGER_H_
#define USER_MANAGER_H_

#include <map>

class CApplication;
class CDNFProhibitUser;

// ---- CUserManager：0x1c ----
class CUserManager
{
public:
    CUserManager();
    ~CUserManager();
    void Init(CApplication* app);
    char InsertProhibitUser(const unsigned int dbid, CDNFProhibitUser* pu);
    CDNFProhibitUser* FindProhibitUser(unsigned int dbid) const;
    bool DeleteProhibitUser(unsigned int dbid);
    void ProcessByMinute();
    std::map<const unsigned int, CDNFProhibitUser*> m_prohibitUsers;  // +0
    CApplication* m_app;  // +0x18
};

#endif  // USER_MANAGER_H_
