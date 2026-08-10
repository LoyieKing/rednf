// df_monitor_r — DNFUserManager（从 MonitorTypes/App/Table 拆分）
#include <stdio.h>
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

#include "DNFUserManager.h"
#include "DNFFileLog.h"
#include "DNFFunctionLib.h"
#include "Packet_Monitor_Member_Secede.h"
#include "Packet_GM_Request_Mid.h"
#include "Packet_PvPChannelInfo.h"
#include "Packet_PvPChannelUserCount.h"
#include "Packet_Item_Limit_Edition_Sell_Start.h"
#include "Packet_DBMW_Change_Char_Name.h"
#include "Packet_Monitor_Reply_Charac_Info.h"
#include "DNFApplication.h"
#include "DNFBuddy.h"
#include "DNFGameServer.h"
#include "DNFPacketTranslater.h"
#include "DNFProhibitUser.h"
#include "DNFServerInterface.h"
#include "DNFUser.h"

CUserManager::CUserManager() {}

CUserManager::~CUserManager()
{
    for (std::map<unsigned int, CUser*>::iterator it = m_users.begin(); it != m_users.end();
         ++it)
    {
        if (it->second != 0)
        {
            delete it->second;
        }
    }
    m_users.clear();
    m_charNoUsers.clear();
    for (std::map<const unsigned int, CDNFProhibitUser*>::const_iterator it =
             m_prohibitUsers.begin();
         it != m_prohibitUsers.end(); ++it)
    {
        if (it->second != 0)
        {
            delete it->second;
        }
    }
    m_prohibitUsers.clear();
}

void CUserManager::Init(CApplication* app) {}

void CUserManager::MemberEnterProcess()
{
    for (std::map<const unsigned int, CUser*>::const_iterator it = m_charNoUsers.begin();
         it != m_charNoUsers.end(); ++it)
    {
        if (it->second != 0)
        {
            it->second->MemberEnterProcess();
        }
    }
}

void CUserManager::ProcessByMinute()
{
    if (!m_prohibitUsers.empty())
    {
        for (std::map<const unsigned int, CDNFProhibitUser*>::iterator it = m_prohibitUsers.begin();
             it != m_prohibitUsers.end(); )
        {
            CDNFProhibitUser* pu = (*it).second;
            register bool hasUser = (pu != 0);
            if (hasUser)
            {
                if (pu->IsTimeOutConnectable())
                {
                    register int remain = (short)pu->GetProhibitRemainTime();
                    register unsigned int dbid = pu->GetDBID();
                    CMyFileLog log("ProcessByMinute", 0x292);
                    log("./log/User",
                        "[PROHIBIT CONNECT USER TIME_OUT] Prohibit User DB ID : %d\t Remain time(%d)\n",
                        dbid, remain);
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
}

CUser* CUserManager::FindUser_CharNo(unsigned int charNo) const
{
    if (!m_charNoUsers.empty())
    {
        std::map<const unsigned int, CUser*>::const_iterator it = m_charNoUsers.find(charNo);
        if (it != m_charNoUsers.end())
        {
            return it->second;
        }
    }
    return 0;
}

CUser* CUserManager::FindUser_CharName(std::string name) const
{
    if (!m_charNameUsers.empty())
    {
        std::map<const std::string, CUser*>::const_iterator it = m_charNameUsers.find(name);
        if (it != m_charNameUsers.end())
        {
            return it->second;
        }
    }
    return 0;
}

void CUserManager::ChangeBlackListCharName(unsigned int dbid, char* name)
{
    if (!m_users.empty())
    {
        for (std::map<unsigned int, CUser*>::const_iterator it = m_users.begin();
             it != m_users.end(); ++it)
        {
            it->second->ChangeCharNameToBlackList(dbid, name);
        }
    }
}

void CUserManager::ResetBlackList(unsigned int charNo)
{
    CUser* user = FindUser(charNo);
    if (user != 0)
    {
        user->ResetBlackList(1);
    }
}

void CUserManager::ResetBuddyList(unsigned int charNo)
{
    CUser* user = FindUser_CharNo(charNo);
    if (user != 0)
    {
        user->ResetBuddyList(true);
    }
}

void CUserManager::DeleteUsersOnGameServerDown(CGameServer* gameServer)
{
    if (!m_charNoUsers.empty())
    {
        for (std::map<const unsigned int, CUser*>::iterator it = m_charNoUsers.begin();
             it != m_charNoUsers.end(); )
        {
            if (it->second != 0 && it->second->GetGameServer() == gameServer)
            {
                std::map<const unsigned int, CUser*>::iterator cur = it++;
                m_charNoUsers.erase(cur);
            }
            else
            {
                ++it;
            }
        }
    }
    if (!m_charNameUsers.empty())
    {
        for (std::map<const std::string, CUser*>::iterator it = m_charNameUsers.begin();
             it != m_charNameUsers.end(); )
        {
            if (it->second != 0 && it->second->GetGameServer() == gameServer)
            {
                std::map<const std::string, CUser*>::iterator cur = it++;
                m_charNameUsers.erase(cur);
            }
            else
            {
                ++it;
            }
        }
    }
    if (!m_users.empty())
    {
        for (std::map<unsigned int, CUser*>::iterator it = m_users.begin();
             it != m_users.end(); )
        {
            CUser* user = it->second;
            if (user != 0 && user->GetGameServer() == gameServer)
            {
                unsigned int key = user->GetUniqCharNo();
                if (key != 0)
                {
                    m_app->Call_DeleteMember(key, user);
                }
                user->GetDBID();
                m_app->Call_ResetBlackList(user->GetUniqCharNo());
                m_app->Call_ResetBuddyList(user->GetUniqCharNo());
                if (user != 0)
                {
                    delete user;
                }
                std::map<unsigned int, CUser*>::iterator cur = it++;
                m_users.erase(cur);
            }
            else
            {
                ++it;
            }
        }
    }
}

void CUserManager::DeleteUsersOnTcpGameServerDown(CTcpGameServer* tcpGameServer)
{
    if (!m_charNoUsers.empty())
    {
        for (std::map<const unsigned int, CUser*>::iterator it = m_charNoUsers.begin();
             it != m_charNoUsers.end(); )
        {
            if (it->second != 0 && it->second->GetTcpGameServer() == (void*)tcpGameServer)
            {
                std::map<const unsigned int, CUser*>::iterator cur = it++;
                m_charNoUsers.erase(cur);
            }
            else
            {
                ++it;
            }
        }
    }
    if (!m_charNameUsers.empty())
    {
        for (std::map<const std::string, CUser*>::iterator it = m_charNameUsers.begin();
             it != m_charNameUsers.end(); )
        {
            if (it->second != 0 && it->second->GetTcpGameServer() == (void*)tcpGameServer)
            {
                std::map<const std::string, CUser*>::iterator cur = it++;
                m_charNameUsers.erase(cur);
            }
            else
            {
                ++it;
            }
        }
    }
    if (!m_users.empty())
    {
        for (std::map<const unsigned int, CUser*>::iterator it = m_users.begin();
             it != m_users.end(); )
        {
            CUser* user = it->second;
            if (user != 0 && user->GetTcpGameServer() == (void*)tcpGameServer)
            {
                unsigned int key = user->GetUniqCharNo();
                if (key != 0)
                {
                    m_app->Call_DeleteMember(key, user);
                }
                user->GetDBID();
                m_app->Call_ResetBlackList(user->GetUniqCharNo());
                m_app->Call_ResetBuddyList(user->GetUniqCharNo());
                if (user != 0)
                {
                    delete user;
                }
                std::map<const unsigned int, CUser*>::iterator cur = it++;
                m_users.erase(cur);
            }
            else
            {
                ++it;
            }
        }
    }
}

void CUserManager::SendConnectedBuddysList(CUser* user)
{
    if (user != 0)
    {
        Packet_Monitor_Reply_Buddy_List pkt;
        int idx = 0;
        CBuddy* buddies[32];
        int count = user->GetBuddys(buddies);
        *(char*)((char*)&pkt + 0xe) = (char)count;
        while (count != 0)
        {
            count--;
            CBuddy* buddy = buddies[idx];
            CUser* buddyUser = FindUser_CharNo(
                *(unsigned int*)((char*)buddy->getBuddyDBInfo() + 0x22));
            if (buddyUser != 0)
            {
                if (buddyUser->GetGameServer() != 0)
                {
                    *(char*)((char*)&pkt + 0xf + idx * 0x2a + 2) = 1;
                    *(char*)((char*)&pkt + 0xf + idx * 0x2a) =
                        ((CServerInterface*)buddyUser->GetGameServer())->GetChannelNo();
                    if (buddyUser->IsBlackUser(user->GetUniqCharNo()) != 0)
                    {
                        *(char*)((char*)&pkt + 0xf + idx * 0x2a + 1) = 1;
                    }
                }
            }
            memcpy((char*)&pkt + 0xf + idx * 0x2a + 3, buddy->getBuddyDBInfo(), 0x27);
            idx++;
        }
        if (idx != 0)
        {
            if (user->GetGameServer() == 0)
            {
                DNF_LOG_SCOPE_LINE(0x34b,"./log/Except",
                    "CPacketTranslater::OnCheckBuddyList : pclCheckUser->GetGameServer() == 0");
            }
            else
            {
                *(unsigned int*)((char*)&pkt + 0xa) = user->GetDBID();
                unsigned short size =
                    (unsigned short)((idx << 2) * 8 + idx * 10 + 0xf);
                ((CServerInterface*)user->GetGameServer())
                    ->SendToServer((char*)&pkt, (unsigned int)size);
            }
        }
    }
}

void CUserManager::GetSchoolCount(unsigned int school, unsigned int* out, unsigned char& idx)
{
    std::map<const unsigned int, std::map<unsigned char, unsigned int> >::iterator it =
        m_mapSchools.find(school);
    DNF_LOG_SCOPE_LINE(0x418, "./log/School", "GetSchoolCount(%u)", school);
    if (it != m_mapSchools.end())
    {
        int pos = 0;
        int n = 0;
        std::map<unsigned char, unsigned int>* inner = &it->second;
        for (std::map<unsigned char, unsigned int>::iterator c = inner->begin();
             c != inner->end() && n < (int)idx; ++c)
        {
            out[pos] = (unsigned int)c->first;
            pos++;
            out[pos] = c->second;
            pos++;
            CMyFileLog log2("GetSchoolCount", 0x423);
            log2("./log/School", "GetSchoolCount(%u) channelNo(%u) Count(%u)", school,
                 out[pos - 2], out[pos - 1]);
            n++;
        }
        idx = (unsigned char)n;
        CMyFileLog log3("GetSchoolCount", 0x426);
        log3("./log/School", "GetSchoolCount(%u) size(%d)", school, (unsigned int)idx);
    }
}

CDNFProhibitUser* CUserManager::FindProhibitUser(unsigned int dbid) const
{
    if (!m_prohibitUsers.empty())
    {
        std::map<const unsigned int, CDNFProhibitUser*>::const_iterator it =
            m_prohibitUsers.find(dbid);
        if (it != m_prohibitUsers.end())
        {
            return it->second;
        }
    }
    return 0;
}

int CUserManager::DeleteProhibitUser(unsigned int dbid, char channel)
{
    if (m_prohibitUsers.empty())
    {
        return 0;
    }
    CDNFProhibitUser* pu = FindProhibitUser(dbid);
    if (pu != 0)
    {
        char puCh = pu->GetChannelNo();
        if (puCh != -1 && puCh != channel)
        {
            DNF_LOG_SCOPE_LINE(0x2c7,"./log/User",
                "[PROHIBIT DELETE USER Err] Disconnected User DB ID : %s, first ch(%d)/complete ch(%d)",
                NumberToString(dbid, 0), (int)puCh, (int)channel);
            return 0;
        }
        char fromWeb = pu->fromWeb();
        if (fromWeb != 0 && channel != -1)
        {
            DNF_LOG_SCOPE_LINE(0x2ce,"./log/User",
                "[PROHIBIT DELETE USER Err From Web] Disconnected User DB ID : %s, first ch(%d)/complete ch(%d)",
                NumberToString(dbid, 0), (int)puCh, (int)channel);
            return 0;
        }
        if (m_prohibitUsers.erase(dbid) == 1)
        {
            if (pu != 0)
            {
                delete pu;
            }
            return 1;
        }
    }
    return 0;
}

CUser* CUserManager::FindUser(unsigned int dbid) const
{
    if (!m_users.empty())
    {
        std::map<const unsigned int, CUser*>::const_iterator it = m_users.find(dbid);
        if (it != m_users.end())
        {
            return it->second;
        }
    }
    return 0;
}

char CUserManager::InsertUser(unsigned int dbid, CUser* user)
{
    if (user == 0)
    {
        return 0;
    }
    m_users.insert(std::pair<const unsigned int, CUser*>(dbid, user));
    return 1;
}

char CUserManager::InsertUser_CharNo(unsigned int charNo, CUser* user)
{
    if (user == 0)
    {
        return 0;
    }
    m_charNoUsers.insert(std::pair<const unsigned int, CUser*>(charNo, user));
    return 1;
}

char CUserManager::InsertUser_CharName(char* name, CUser* user)
{
    if (user == 0)
    {
        return 0;
    }
    bool ok;
    {
        std::pair<const std::string, CUser*> p(name, user);
        ok = m_charNameUsers.insert(p).second;
    }
    if (ok)
    {
        return 1;
    }
    CMyFileLog log("InsertUser_CharName", 0x211);
    log("./log/Except", "[INSERT_ERR]Already Exist!\tChar Name : %s\tDB No : %d\n",
        name, user->GetDBID());
    return 0;
}

CUser* CUserManager::CreateUser(unsigned int dbid, unsigned int charNo, char* charName,
                                int channel, CGameServer* server)
{
    CUser* user = new CUser;
    user->SetDBID(dbid);
    user->SetUniqCharNo(charNo);
    user->SetIdByChannel(channel);
    user->SetGameServer(server);
    if (InsertUser(dbid, user) != 1)
    {
        DNF_LOG_SCOPE_AT("CreateUser", 0x1a9,"./log/LoginErr",
            "uDBID(%s) uCharNo(%d) is already exist at m_mapUsers!", NumberToString(dbid, 0),
            charNo);
    }
    user->SetUserPosState(2);
    if (charNo != 0)
    {
        if (InsertUser_CharNo(charNo, user) != 1)
        {
            DNF_LOG_SCOPE_AT("CreateUser", 0x1b3,"./log/LoginErr",
                "uDBID(%s) uCharNo(%d) is already exist at m_mapCharNoUsers!",
                NumberToString(dbid, 0), charNo);
        }
        if (InsertUser_CharName(charName, user) != 1)
        {
            DNF_LOG_SCOPE_AT("CreateUser", 0x1b7,"./log/LoginErr",
                "uDBID(%s) uCharName(%s) is already exist at m_mapCharNameUsers!",
                NumberToString(dbid, 0), charName);
        }
        user->SetUserPosState(3);
    }
    return user;
}

char CUserManager::InsertProhibitUser(unsigned int dbid, CDNFProhibitUser* pu)
{
    if (pu == 0)
    {
        return 0;
    }
    m_prohibitUsers.insert(std::pair<const unsigned int, CDNFProhibitUser*>(dbid, pu));
    return 1;
}

int CUserManager::DeleteUser(unsigned int dbid)
{
    if (m_users.empty())
    {
        return 0;
    }
    CUser* user = FindUser(dbid);
    if (user != 0)
    {
        if (user->GetGameServer() == 0)
        {
            return 0;
        }
        CDNFProhibitUser* pu = new CDNFProhibitUser;
        char ch = ((CServerInterface*)user->GetGameServer())->GetChannelNo();
        pu->SetUserConnectableTime(dbid, 10, ch, false);
        if (InsertProhibitUser(dbid, pu) != 1)
        {
            DNF_LOG_SCOPE_LINE(0x8b,"./log/ProhibitUser",
                "[INSERT_ERR_] CUserManager::DeleteUser() m_id : %s, time( %d ), Channel( %d )\n",
                NumberToString(dbid, 0), 10, (unsigned int)ch & 0xff);
            delete pu;
        }
        if (m_users.erase(dbid) == 1)
        {
            if (user != 0)
            {
                delete user;
            }
            return 1;
        }
    }
    return 0;
}

int CUserManager::DeleteUser(CUser* user)
{
    if (m_users.empty())
    {
        return 0;
    }
    if (user != 0)
    {
        if (user->GetGameServer() == 0)
        {
            return 0;
        }
        unsigned int dbid = user->GetDBID();
        CDNFProhibitUser* pu = new CDNFProhibitUser;
        char ch = ((CServerInterface*)user->GetGameServer())->GetChannelNo();
        pu->SetUserConnectableTime(dbid, 10, ch, false);
        if (InsertProhibitUser(dbid, pu) != 1)
        {
            DNF_LOG_SCOPE_LINE(0xc4,"./log/ProhibitUser",
                "[INSERT_ERR_] CUserManager::DeleteUser() m_id : %s, time( %d ), Channel( %d )\n",
                NumberToString(dbid, 0), 10, (unsigned int)ch & 0xff);
            delete pu;
        }
        if (m_users.erase(dbid) == 1)
        {
            if (user != 0)
            {
                delete user;
            }
            return 1;
        }
    }
    return 0;
}

void CUserManager::AddSchoolNo(unsigned int schoolNo, unsigned char channel)
{
    std::map<const unsigned int, std::map<unsigned char, unsigned int> >::iterator it =
        m_mapSchools.find(schoolNo);
    if (it != m_mapSchools.end())
    {
        std::map<unsigned char, unsigned int>* inner = &it->second;
        std::map<unsigned char, unsigned int>::iterator c = inner->find(channel);
        if (c != inner->end())
        {
            c->second++;
            DNF_LOG_SCOPE_LINE(0x3f9,"./log/School",
                "3) AddSchoolNo(%d, %d), mapSchoolChannel.size(%u), m_mapSchools.size(%u)",
                schoolNo, channel, inner->size(), m_mapSchools.size());
        }
        else
        {
            inner->insert(std::pair<unsigned char, unsigned int>(channel, 1));
            DNF_LOG_SCOPE_LINE(0x3f3, "./log/School",
                "2) AddSchoolNo(%d, %d), mapSchoolChannel.size(%u), m_mapSchools.size(%u)",
                schoolNo, channel, inner->size(), m_mapSchools.size());
        }
    }
    else
    {
        std::map<unsigned char, unsigned int> newInner;
        newInner.insert(std::pair<unsigned char, unsigned int>(channel, 1));
        m_mapSchools.insert(
            std::pair<const unsigned int, std::map<unsigned char, unsigned int> >(schoolNo,
                                                                                 newInner));
        DNF_LOG_SCOPE_LINE(0x3ed, "./log/School",
            "1) AddSchoolNo(%d, %d), mapSchoolChannel.size(%u), m_mapSchools.size(%u)",
            schoolNo, channel, newInner.size(), m_mapSchools.size());
    }
}

void CUserManager::DeleteBlackUserOnCharacDelete(unsigned int charNo)
{
    if (!m_users.empty())
    {
        for (std::map<const unsigned int, CUser*>::iterator it = m_users.begin();
             it != m_users.end(); ++it)
        {
            if (it->second != 0)
            {
                it->second->DeleteToBlackList(charNo);
            }
        }
    }
}

unsigned int CUserManager::GetSizeOfCharnoUsers()
{
    return (unsigned int)m_charNoUsers.size();
}

unsigned int CUserManager::Size()
{
    return (unsigned int)m_users.size();
}

int CUserManager::DeleteUser_CharNo(unsigned int charNo)
{
    if (m_charNoUsers.empty() || charNo == 0)
    {
        return 0;
    }
    if (m_charNoUsers.erase(charNo) == 1)
    {
        return 1;
    }
    DNF_LOG_SCOPE_LINE(0x1eb, "./log/User",
        "[EXCEPT]CUserManager::DeleteUser_CharNo() : Erase Fail!\tChar No : %d\tChar_No Map "
        "Count : %d\n",
        charNo, (unsigned int)m_charNoUsers.size());
    return 0;
}

int CUserManager::DeleteUser_CharName(std::string name)
{
    if (m_charNameUsers.empty() || name.empty())
    {
        return 0;
    }
    if (m_charNameUsers.erase(name) == 1)
    {
        return 1;
    }
    DNF_LOG_SCOPE_LINE(0x22c,"./log/Except",
        "[EXCEPT]CUserManager::DeleteUser_CharNo() : Erase Fail!\tChar Name : %s\tChar_No Map "
        "Count : %d\n",
        name.c_str(), (unsigned int)m_charNoUsers.size());
    return 0;
}

void CUserManager::DelSchoolNo(unsigned int schoolNo, unsigned char channel)
{
    std::map<const unsigned int, std::map<unsigned char, unsigned int> >::iterator it =
        m_mapSchools.find(schoolNo);
    if (it != m_mapSchools.end())
    {
        std::map<unsigned char, unsigned int>* inner = &it->second;
        std::map<unsigned char, unsigned int>::iterator c = inner->find(channel);
        if (c != inner->end())
        {
            c->second--;
            if (c->second == 0)
            {
                inner->erase(c);
                if (inner->size() == 0)
                {
                    m_mapSchools.erase(it);
                }
            }
            unsigned int outerSize = m_mapSchools.size();
            unsigned int innerSize = inner->size();
            DNF_LOG_SCOPE_LINE(0x40f,"./log/School",
                "DelSchoolNo(%d, %d), mapSchoolChannel.size(%u), m_mapSchools.size(%u)",
                schoolNo, channel, innerSize, outerSize);
        }
    }
}

void CUserManager::ResetMemberInfo(unsigned int charNo)
{
    CUser* user = FindUser_CharNo(charNo);
    if (user != 0)
    {
        user->ResetMemberInfo();
    }
}

