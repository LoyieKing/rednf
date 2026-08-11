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

void CMemoryCashManager::ProcessCashDataPrint()
{
    m_cashCnt34 = m_cashCnt34 - 1;
    if (m_cashCnt34 <= 0)
    {
        CMyFileLog log("ProcessCashDataPrint", 0x4e);
        log("./log/cashmem", "buddy(%d/%d)  member(%d/%d)  black(%d/%d)\n",
            m_field44, m_buddyCashCnt, m_field48, m_memberCashCnt, m_field4c,
            m_blackListCashCnt);
        resetCashCnt();
    }
}

char CMemoryCashManager::QueryCashMemoryMember(CUser* user)
{
    char ok = 0;
    if (!m_cashObjects.empty())
    {
        unsigned int dbid = user->GetDBID();
        std::map<unsigned int, CCashObject*>::iterator it = m_cashObjects.find(dbid);
        if (it != m_cashObjects.end())
        {
            CCashObject* obj = it->second;
            if (obj->GetCharacNo() == user->GetUniqCharNo())
            {
                CMemberManager* mgr = m_app->Get_MemberManager();
                CMember* member = obj->GetMemberObject();
                if (member != 0)
                {
                    unsigned int* dbInfo = member->GetMemberDBInfoW();
                    std::string name;
                    if (QueryUpdatedCharacName(*dbInfo, name))
                    {
                        memset((char*)dbInfo + 5 * 4, 0, 0x1e);
                        strncpy((char*)dbInfo + 5 * 4, name.c_str(), 0x1d);
                    }
                    for (int i = 0; i < (int)((RA_U8<39>*)dbInfo)->v; i++)
                    {
                        unsigned int* sub =
                            (unsigned int*)((char*)dbInfo + i * 0x27 + 0x28);
                        if (*sub != 0)
                        {
                            if (QueryUpdatedCharacName(*sub, name))
                            {
                                memset((char*)sub + 5 * 4, 0, 0x1e);
                                strncpy((char*)sub + 5 * 4, name.c_str(), 0x1d);
                            }
                        }
                    }
                    ok = mgr->LoadMemberFromCash(user, member);
                    if (ok != 0)
                    {
                        incMemberCashHitCnt();
                    }
                    obj->ClearMemberObject();
                }
                else
                {
                    obj->DeleteMemberObject();
                    ok = 0;
                }
            }
        }
    }
    return ok;
}

int CMemoryCashManager::QueryCashMemoryBuddyInfo(CUser* user)
{
    if (!m_cashObjects.empty())
    {
        unsigned int dbid = user->GetDBID();
        std::map<unsigned int, CCashObject*>::iterator it = m_cashObjects.find(dbid);
        if (it != m_cashObjects.end())
        {
            CCashObject* obj = it->second;
            if (obj->GetCharacNo() == user->GetUniqCharNo())
            {
                CBuddy* buddies[32];
                int count = obj->GetBuddysObject(buddies);
                for (int i = 0; i < count; i++)
                {
                    if (buddies[i] != 0)
                    {
                        std::string name;
                        unsigned int* info = buddies[i]->getBuddyDBInfo();
                        if (QueryUpdatedCharacName(((RA_UINT<34>*)info)->v, name))
                        {
                            memset(info, 0, 0x1e);
                            strncpy((char*)info, name.c_str(), 0x1d);
                        }
                        user->AddBuddyFromCash(buddies[i]);
                        user->GetUniqCharNo();
                        unsigned int charNo = ((RA_UINT<34>*)info)->v;
                        m_app->Get_BuddyRegisterManager()->addBuddyRegister(charNo,
                                                                            user->GetUniqCharNo());
                    }
                }
                if (count != 0)
                {
                    m_app->Get_UserManager()->SendConnectedBuddysList(user);
                }
                user->SetBuddyDBFlag(4);
                incBuddyCashHitCnt();
                return 1;
            }
            obj->DeleteBuddys();
            return 0;
        }
    }
    return 0;
}

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
                        memset(buf, 0, 30);
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

char CMemoryCashManager::QueryUpdatedCharacName(unsigned int charNo, std::string& name)
{
    std::map<unsigned int, std::string>::iterator it = m_names.find(charNo);
    if (it != m_names.end())
    {
        name = it->second;
        return 1;
    }
    return 0;
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
    m_field48 = m_field48 + 1;
}

void CMemoryCashManager::incBuddyCashHitCnt()
{
    m_field44 = m_field44 + 1;
}

void CMemoryCashManager::incBlackListCashHitCnt()
{
    m_field4c = m_field4c + 1;
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
    flag1 = false;
    flag3 = false;
    flag2 = false;
    char ret = 0;
    std::map<unsigned int, CCashObject*>::iterator it = m_cashObjects.find(user->GetDBID());
    if (it == m_cashObjects.end())
    {
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
        if (user->GetMemberKey() != 0 && (user->GetMemberDBFlag() & 4) != 0)
        {
            if (member != 0 && member->IsAbleToRegisterMember() != 0)
            {
                flag2 = true;
                ret = 1;
            }
        }
        return ret;
    }
    return 0;
}

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
        m_cashObjects.insert(
            std::pair<const unsigned int, CCashObject*>(user->GetDBID(), obj));
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
    }
    return it != m_cashObjects.end();
}

void CMemoryCashManager::DeleteCashObjecct(unsigned int dbid)
{
    std::map<unsigned int, CCashObject*>::iterator it = m_cashObjects.find(dbid);
    if (it != m_cashObjects.end())
    {
        if (it->second != 0)
        {
            delete it->second;
        }
        m_cashObjects.erase(it);
    }
}

void CMemoryCashManager::resetCashCnt()
{
    m_cashCnt34 = 0x1e;
    m_buddyCashCnt = 0;
    m_memberCashCnt = 0;
    m_blackListCashCnt = 0;
    m_field44 = 0;
    m_field48 = 0;
    m_field4c = 0;
}
