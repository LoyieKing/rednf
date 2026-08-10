// df_manager_r — CUserManager（ORIG UserManager.cpp）
#include "UserManager.h"

#include <utility>

#include "DNFFileLog.h"
#include "ProhibitUser.h"

CUserManager::CUserManager()
{
    m_app = 0;
}

CUserManager::~CUserManager()
{
    for (std::map<const unsigned int, CDNFProhibitUser*>::const_iterator it = m_prohibitUsers.begin();
         it != m_prohibitUsers.end(); ++it)
    {
        CDNFProhibitUser* pu = it->second;
        if (pu)
        {
            delete pu;
        }
    }
    m_prohibitUsers.clear();
}

void CUserManager::Init(CApplication* app)
{
    m_app = app;
}

char CUserManager::InsertProhibitUser(const unsigned int dbid, CDNFProhibitUser* pu)
{
    if (!pu)
        return 0;
    return m_prohibitUsers.insert(std::make_pair(dbid, pu)).second;
}

CDNFProhibitUser* CUserManager::FindProhibitUser(unsigned int dbid) const
{
    std::map<const unsigned int, CDNFProhibitUser*>::const_iterator it = m_prohibitUsers.find(dbid);
    if (it == m_prohibitUsers.end())
        return 0;
    return it->second;
}

char CUserManager::DeleteProhibitUser(unsigned int dbid)
{
    if (m_prohibitUsers.empty())
        return 0;
    CDNFProhibitUser* pu = FindProhibitUser(dbid);
    if (pu && m_prohibitUsers.erase(dbid) == 1)
    {
        delete pu;
        return 1;
    }
    return 0;
}

void CUserManager::ProcessByMinute()
{
    if (!m_prohibitUsers.empty())
    {
        for (std::map<const unsigned int, CDNFProhibitUser*>::iterator it = m_prohibitUsers.begin();
             it != m_prohibitUsers.end();)
        {
            CDNFProhibitUser* pu = (*it).second;
            if (pu && pu->IsTimeOutWaitMonitor())
            {
                CMyFileLog log("ProcessByMinute", 0x43);
                log("./log/ProhibitUser",
                    "[PROHIBIT CONNECT USER TIME_OUT] Prohibit User DB ID : %d. Remain time(%d)\n",
                    pu->GetDBID(), pu->GetProhibitRemainTime());
                delete pu;
                m_prohibitUsers.erase(it++);
            }
            else
            {
                ++it;
            }
        }
    }
}
