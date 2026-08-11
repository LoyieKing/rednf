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
        if (it->second)
            delete it->second;
    }
    m_prohibitUsers.clear();
}

void CUserManager::Init(CApplication* app)
{
    m_app = app;
}

char CUserManager::InsertProhibitUser(const unsigned int dbid, CDNFProhibitUser* pu)
{
    if (pu)
        return m_prohibitUsers.insert(std::make_pair(dbid, pu)).second;
    return 0;
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
    CDNFProhibitUser* pu;
    // R10: ORIG 结构：嵌套 if（裸指针判空）→ erase==1 → delete 内建判空。
    if ((pu = FindProhibitUser(dbid)) != 0)
    {
        if (m_prohibitUsers.erase(dbid) == 1)
        {
            // R10: ORIG 为 delete 内建空检查形态（mov mem,%ebx; test; je）。
            delete pu;
            return 1;
        }
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
            CDNFProhibitUser* pu;
            // R10: ORIG 结构——pu==0 时直接跳到 end 检查（不复位迭代器），
            // timeout 分支用 continue 跳过 ++it；pu 用 (pu=..)!=0 形态复现 setne。
            if ((pu = (*it).second) != 0)
            {
                if (pu->IsTimeOutWaitMonitor())
                {
                    DNF_LOG_SCOPE_LINE(0x43, "./log/ProhibitUser",
                        "[PROHIBIT CONNECT USER TIME_OUT] Prohibit User DB ID : %d\t Remain time(%d)\n",
                        pu->GetDBID(), pu->GetProhibitRemainTime());
                    delete pu;
                    m_prohibitUsers.erase(it++);
                    continue;
                }
                ++it;
            }
        }
    }
}
