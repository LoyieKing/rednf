// df_monitor_r — MemoryCashManager（从 MonitorTypes/App/Table 拆分）
#include <stdio.h>
#include "RawAccess.h"
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <cerrno>
#include <signal.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <sys/stat.h>
#include <sys/times.h>
#include <algorithm>

#include "MemoryCashManager.h"
#include "DNFFileLog.h"
#include "DNFFunctionLib.h"
#include "Packet_Monitor_Member_Secede.h"
#include "Packet_GM_Request_Mid.h"
#include "Packet_PvPChannelInfo.h"
#include "Packet_PvPChannelUserCount.h"
#include "Packet_Item_Limit_Edition_Sell_Start.h"
#include "Packet_DBMW_Change_Char_Name.h"
#include "Packet_Monitor_Reply_Charac_Info.h"
#include "BlackUser.h"
#include "CashObject.h"
#include "DNFApplication.h"
#include "DNFBuddy.h"
#include "DNFMember.h"
#include "DNFMemberManager.h"
#include "DNFUser.h"

void CMemoryCashManager::Init(CApplication* app)
{
    m_app = app;
    resetCashCnt();
}

CMemoryCashManager::CMemoryCashManager()
{
    m_app = 0;
}

CMemoryCashManager::~CMemoryCashManager() {}

// [DNF-NONIDENTICAL] DNF-MON-DIFF-0117 | monitor | 与ORIG差异=DIFF | CMemoryCashManager::ProcessLifeTimeOut | 详见 function_reports/monitor/_ZN18CMemoryCashManager18ProcessLifeTimeOutEv.md
void CMemoryCashManager::ProcessLifeTimeOut()
{
    if (m_app)
    {
        for (std::map<unsigned int, CCashObject*>::iterator it = m_cashObjects.begin();
             it != m_cashObjects.end(); )
        {
            CCashObject* obj = it->second;
            if (obj->IsLifeTimeOut())
            {
                obj->DeleteMemberObject();
                obj->DeleteBuddys();
                obj->DeleteBlackUsers();
                m_cashObjects.erase(it++);
                delete obj;
            }
            else
            {
                ++it;
            }
        }
    }
}

// [DNF-NONIDENTICAL] DNF-MON-DIFF-0118 | monitor | 与ORIG差异=DIFF | CMemoryCashManager::QueryCashMemoryMember | 详见 function_reports/monitor/_ZN18CMemoryCashManager21QueryCashMemoryMemberEP5CUser.md
char CMemoryCashManager::QueryCashMemoryMember(CUser* user)
{
    if (m_cashObjects.empty())
    {
        return 0;
    }
    unsigned int dbid = user->GetDBID();
    std::map<unsigned int, CCashObject*>::iterator it = m_cashObjects.find(dbid);
    if (it != m_cashObjects.end())
    {
        CCashObject* obj = it->second;
        if (obj->GetCharacNo() != user->GetUniqCharNo())
        {
            obj->DeleteMemberObject();
            return 0;
        }
        CMemberManager* mgr = m_app->Get_MemberManager();
        CMember* member = obj->GetMemberObject();
        if (member != 0)
        {
            unsigned int* dbInfo = member->GetMemberDBInfoW();
            std::string name;
            if (QueryUpdatedCharacName(*dbInfo, name))
            {
                memset((char*)dbInfo + 5, 0, 0x1e);
                strncpy((char*)dbInfo + 5, name.c_str(), 0x1d);
            }
            for (int i = 0; i < (int)((STMemberDBInfo*)dbInfo)->m_count27; i++)
            {
                unsigned int* sub = (unsigned int*)&((STMemberDBInfo*)dbInfo)->m_lowers[i];
                if (*sub != 0)
                {
                    if (QueryUpdatedCharacName(*sub, name))
                    {
                        memset((char*)sub + 5, 0, 0x1e);
                        strncpy((char*)sub + 5, name.c_str(), 0x1d);
                    }
                }
            }
        }
        char ok = mgr->LoadMemberFromCash(user, obj->GetMemberObject());
        if (ok != 0)
        {
            incMemberCashHitCnt();
        }
        obj->ClearMemberObject();
        return ok;
    }
    return 0;
}

// [DNF-NONIDENTICAL] DNF-MON-DIFF-0120 | monitor | 与ORIG差异=DIFF | CMemoryCashManager::QueryCashMemoryBuddyInfo | 详见 function_reports/monitor/_ZN18CMemoryCashManager24QueryCashMemoryBuddyInfoEP5CUser.md
int CMemoryCashManager::QueryCashMemoryBuddyInfo(CUser* user)
{
    if (m_cashObjects.empty())
    {
        return 0;
    }
    unsigned int dbid = user->GetDBID();
    std::map<unsigned int, CCashObject*>::iterator it = m_cashObjects.find(dbid);
    if (it != m_cashObjects.end())
    {
        CCashObject* obj = it->second;
        if (obj->GetCharacNo() != user->GetUniqCharNo())
        {
            obj->DeleteBuddys();
            return 0;
        }
        CBuddy* buddies[32];
        int count = obj->GetBuddysObject(buddies);
        for (int i = 0; i < count; i++)
        {
            if (buddies[i] != 0)
            {
                std::string name;
                if (QueryUpdatedCharacName(
                        ((STBuddyDBInfo*)buddies[i]->getBuddyDBInfo())->m_characNo, name))
                {
                    memset(buddies[i]->getBuddyDBInfo(), 0, 0x1e);
                    strncpy((char*)buddies[i]->getBuddyDBInfo(), name.c_str(), 0x1d);
                }
                user->AddBuddyFromCash(buddies[i]);
                register unsigned int uniqNo = user->GetUniqCharNo();
                register unsigned int charNo =
                    ((STBuddyDBInfo*)buddies[i]->getBuddyDBInfo())->m_characNo;
                m_app->Get_BuddyRegisterManager()->addBuddyRegister(charNo, uniqNo);
            }
        }
        if (count != 0)
        {
            CUserManager* mgr = m_app->Get_UserManager();
            mgr->SendConnectedBuddysList(user);
        }
        user->SetBuddyDBFlag(4);
        incBuddyCashHitCnt();
        return 1;
    }
    return 0;
}

// [DNF-NONIDENTICAL] DNF-MON-DIFF-0119 | monitor | 与ORIG差异=DIFF | CMemoryCashManager::QueryCashMemoryBlackList | 详见 function_reports/monitor/_ZN18CMemoryCashManager24QueryCashMemoryBlackListEP5CUser.md
char CMemoryCashManager::QueryCashMemoryBlackList(CUser* user)
{
    unsigned int dbid = user->GetDBID();
    std::map<unsigned int, CCashObject*>::iterator it = m_cashObjects.find(dbid);
    if (it != m_cashObjects.end())
    {
        CCashObject* obj = it->second;
        if (obj != 0)
        {
            std::map<unsigned int, CBlackUser*>* blackMap = obj->GetBlackUsersObject();
            for (std::map<unsigned int, CBlackUser*>::iterator bi = blackMap->begin();
                 bi != blackMap->end(); ++bi)
            {
                unsigned int key = (*bi).first;
                CBlackUser* bu = (*bi).second;
                if (bu != 0)
                {
                    std::string name;
                    if (QueryUpdatedCharacName(key, name))
                    {
                        char buf[30];
                        __builtin_memset(buf, 0, 0x1e);
                        strncpy(buf, name.c_str(), 0x1d);
                        bu->ChangeCharName(buf);
                    }
                }
            }
            user->RegisterToCashBlackList(*obj->GetBlackUsersObject());
            obj->ClearMapBlackUsers();
            user->SetBlackListDBFlag(4);
            incBlackListCashHitCnt();
            return 1;
        }
    }
    return 0;
}

bool CMemoryCashManager::QueryUpdatedCharacName(unsigned int charNo, std::string& name)
{
    std::map<unsigned int, std::string>::iterator it = m_names.find(charNo);
    if (it == m_names.end())
    {
        return false;
    }
    name = it->second;
    return true;
}

void CMemoryCashManager::InsertUpdatedCharacName(unsigned int dbid, const std::string& name)
{
    std::map<unsigned int, std::string>::iterator it = m_names.find(dbid);
    if (it != m_names.end())
    {
        it->second = name;
    }
    else
    {
        m_names.insert(std::pair<const unsigned int, std::string>(dbid, name));
    }
}

void CMemoryCashManager::incMemberCashHitCnt()
{
    m_memberCashHitCnt = m_memberCashHitCnt + 1;
}

void CMemoryCashManager::incBuddyCashHitCnt()
{
    m_buddyCashHitCnt = m_buddyCashHitCnt + 1;
}

void CMemoryCashManager::incBlackListCashHitCnt()
{
    m_blackListCashHitCnt = m_blackListCashHitCnt + 1;
}

void CMemoryCashManager::incBuddyCashCnt()
{
    m_buddyCashCnt = m_buddyCashCnt + 1;
}

void CMemoryCashManager::incMemberCashCnt()
{
    m_memberCashCnt = m_memberCashCnt + 1;
}

void CMemoryCashManager::incBlackListCashCnt()
{
    m_blackListCashCnt = m_blackListCashCnt + 1;
}

char CMemoryCashManager::IsRightObject(CUser* user, CMember* member, bool& flag1, bool& flag2,
                                       bool& flag3)
{
    char ret = 0;
    flag1 = false;
    flag3 = false;
    flag2 = false;
    std::map<unsigned int, CCashObject*>::iterator it = m_cashObjects.find(user->GetDBID());
    if (it != m_cashObjects.end())
    {
        return 0;
    }
    if ((user->GetBuddyDBFlag() & 4) != 0)
    {
        flag1 = true;
        ret = 1;
    }
    if ((user->GetBlackListDBFlag() & 4) != 0)
    {
        flag3 = true;
        ret = 1;
    }
    if (user->GetMemberKey() != 0)
    {
        if ((user->GetMemberDBFlag() & 4) != 0)
        {
            if (member != 0 && member->IsAbleToRegisterMember() != 0)
            {
                flag2 = true;
                ret = 1;
            }
        }
    }
    return ret;
}

// [DNF-NONIDENTICAL] DNF-MON-DIFF-0121 | monitor | 与ORIG差异=DIFF | CMemoryCashManager::InsertCashMemorySetCharacterObject | 详见 function_reports/monitor/_ZN18CMemoryCashManager34InsertCashMemorySetCharacterObjectEP5CUserP7CMemberRbS4_.md
int CMemoryCashManager::InsertCashMemorySetCharacterObject(CUser* user, CMember* member,
                                                           bool& flag1, bool& flag2)
{
    bool local = false;
    if (IsRightObject(user, member, flag1, flag2, local) == 0)
    {
        return 0;
    }
    CCashObject* obj = new CCashObject;
    if (flag1 || flag2)
    {
        obj->SetCharacNo(user->GetUniqCharNo());
    }
    if (flag2 && member != 0)
    {
        obj->SetMemberObject(member);
        incMemberCashCnt();
    }
    if (flag1)
    {
        CBuddy* buddies[0x20];
        int n = user->GetBuddys(buddies);
        obj->SetBuddysObject(buddies, n);
        incBuddyCashCnt();
    }
    std::pair<std::map<unsigned int, CCashObject*>::iterator, bool> r =
        m_cashObjects.insert(std::make_pair(user->GetDBID(), obj));
    if (r.second)
    {
        return 1;
    }
    delete obj;
    return 0;
}

bool CMemoryCashManager::SetUserObject(CUser* user)
{
    std::map<unsigned int, CCashObject*>::iterator it = m_cashObjects.find(user->GetDBID());
    if (it != m_cashObjects.end())
    {
        CCashObject* obj = it->second;
        obj->SetBlackUsersObject(*user->GetMapBlackList());
        incBlackListCashCnt();
        obj->SetLifeTime(5);
        return 1;
    }
    return 0;
}

void CMemoryCashManager::DeleteCashObjecct(unsigned int dbid)
{
    std::map<unsigned int, CCashObject*>::iterator it = m_cashObjects.find(dbid);
    if (it != m_cashObjects.end())
    {
        delete it->second;
        m_cashObjects.erase(it);
    }
}

void CMemoryCashManager::resetCashCnt()
{
    m_cashCnt34 = 0x1e;
    m_buddyCashCnt = 0;
    m_memberCashCnt = 0;
    m_blackListCashCnt = 0;
    m_buddyCashHitCnt = 0;
    m_memberCashHitCnt = 0;
    m_blackListCashHitCnt = 0;
}
