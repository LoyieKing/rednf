// df_guild_r — DNFUserManager (split from source/guild per ORIG layout)
#include <string.h>
#include <time.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <fcntl.h>
#include <unistd.h>
#include "tinyxml.h"
#include "Packet_Monitor_Call_Guild_Members_ToChannel.h"
#include "Packet_Monitor_Call_Guild_Members_ToChannel_Next.h"
#include "Packet_Monitor_Call_Guild_All_Members_ToChannel.h"
#include "Packet_Monitor_Call_Guild_All_Members_ToChannel_Next.h"
#include "DNFFileLog.h"
#include "DNFFunctionLib.h"

#include "DNFUserManager.h"
#include "BlackUser.h"
#include "CashObject.h"
#include "DNFAppConfig.h"
#include "DNFAppStartInit.h"
#include "DNFAppStopInit.h"
#include "DNFApplication.h"
#include "DNFDBServer.h"
#include "DNFFloatingPointExceptSig.h"
#include "DNFGameServer.h"
#include "DNFGuild.h"
#include "DNFGuildManager.h"
#include "DNFGuildServerMain.h"
#include "DNFGuildWar.h"
#include "DNFInnerMsgHandler.h"
#include "DNFKillUserConfig.h"
#include "DNFManagerServer.h"
#include "DNFMonitorServer.h"
#include "DNFNetworkThread.h"
#include "DNFPacketBuffer.h"
#include "DNFPacketDecoder.h"
#include "DNFPacketTracer.h"
#include "DNFPacketTranslater.h"
#include "DNFPowerWarConfig.h"
#include "DNFSegmentationFaultSig.h"
#include "DNFServerConfig.h"
#include "DNFServerHandler.h"
#include "DNFServerInterface.h"
#include "DNFSignal.h"
#include "DNFSignalTranslator.h"
#include "DNFSystemFailSig.h"
#include "DNFTableBase.h"
#include "DNFTcpAcceptThread.h"
#include "DNFTcpHandler.h"
#include "DNFTcpNetworkThread.h"
#include "DNFTcpSocket.h"
#include "DNFTerminateSig.h"
#include "DNFThreadInterface.h"
#include "DNFTickHandler.h"
#include "DNFUdpHandler.h"
#include "DNFUser.h"
#include "DNFUser1Sig.h"
#include "DNFVersion.h"
#include "GMAccounts.h"
#include "GuildBoard.h"
#include "GuildCargo.h"
#include "MemPool.h"
#include "MemoryCashManager.h"
#include "PacketCounter.h"
#include "Peer.h"
#include "Power.h"
#include "PowerManager.h"
#include "PowerWar.h"
#include "PowerWarCharacInfo.h"
#include "PowerWarGuildInfo.h"
#include "Scheduler.h"
#include "ServerLoadChecker.h"
#include "ServerXml.h"
#include "SystemTimeHandler.h"
#include "TcpNetSystem.h"
#include "WebEvent.h"

void CUserManager::Process()
{
}

CUserManager::CUserManager()
{
    m_app = 0;
}

CUserManager::~CUserManager()
{
    for (std::map<unsigned int, CUser*>::iterator it = m_users.begin();
         it != m_users.end(); ++it)
    {
        if (it->second != 0)
        {
            delete it->second;
        }
    }
    m_users.clear();
    m_charNoUsers.clear();
}

void CUserManager::Init(CApplication* app)
{
    m_app = app;
}

bool CUserManager::InsertUser(const unsigned int dbid, CUser* user)
{
    if (user != 0)
    {
        return m_users.insert(std::make_pair(dbid, user)).second;
    }
    return 0;
}

void CUserManager::DeleteUser(unsigned int dbid)
{
    if (m_users.empty())
    {
        return;
    }
    CUser* user = FindUser(dbid);
    if (user != 0)
    {
        if (user->GetGameServer() == 0)
        {
            return;
        }
        if (m_users.erase(dbid) == 1)
        {
            char* mid = NumberToString(dbid, 0);
            DNF_LOG_SCOPE_LINE(0x5f,"./log/User",
                "[USER LOGOUT] Disconnected User DB ID : %s, Char No : %d , char name:%s\n",
                mid, user->GetUniqCharNo(), user->GetCharName());
            delete user;
        }
    }
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
        if (m_users.erase(dbid) == 1)
        {
            register char* mid = NumberToString(dbid, 0);
            DNF_LOG_SCOPE_LINE(0x7d, "./log/User", "[USER LOGOUT] Disconnected User DB ID : %s\n", mid);
            delete user;
            return 1;
        }
    }
    return 0;
}

void CUserManager::DeleteUsersOnTcpGameServerDown(CTcpGameServer* server)
{
    if (m_users.empty())
    {
        return;
    }
    for (std::map<unsigned int, CUser*>::iterator it = m_charNoUsers.begin();
         it != m_charNoUsers.end();)
    {
        if (it->second != 0)
        {
            if (it->second->GetTcpGameServer() == server)
            {
                m_charNoUsers.erase(it++);
                continue;
            }
        }
        ++it;
    }
    for (std::map<std::string, CUser*>::iterator it = m_charNameUsers.begin();
         it != m_charNameUsers.end();)
    {
        if (it->second != 0)
        {
            if (it->second->GetTcpGameServer() == server)
            {
                m_charNameUsers.erase(it++);
                continue;
            }
        }
        ++it;
    }
    for (std::map<unsigned int, CUser*>::iterator it = m_users.begin();
         it != m_users.end();)
    {
        CUser* user = it->second;
        if (user != 0)
        {
            if (user->GetTcpGameServer() == server)
            {
                unsigned int guildKey = user->GetGuildKey();
                if (guildKey != 0)
                {
                    m_app->Call_DeleteGuildMember(guildKey, user);
                }
                delete user;
                m_users.erase(it++);
                continue;
            }
        }
        ++it;
    }
}

void CUserManager::DeleteUsersOnGameServerDown(CGameServer* server)
{
    if (m_users.empty())
    {
        return;
    }
    for (std::map<unsigned int, CUser*>::iterator it = m_charNoUsers.begin();
         it != m_charNoUsers.end();)
    {
        if (it->second != 0)
        {
            if (it->second->GetGameServer() == server)
            {
                m_charNoUsers.erase(it++);
                continue;
            }
        }
        ++it;
    }
    for (std::map<std::string, CUser*>::iterator it = m_charNameUsers.begin();
         it != m_charNameUsers.end();)
    {
        if (it->second != 0)
        {
            if (it->second->GetGameServer() == server)
            {
                m_charNameUsers.erase(it++);
                continue;
            }
        }
        ++it;
    }
    for (std::map<unsigned int, CUser*>::iterator it = m_users.begin();
         it != m_users.end();)
    {
        CUser* user = it->second;
        if (user != 0)
        {
            if (user->GetGameServer() == server)
            {
                unsigned int guildKey = user->GetGuildKey();
                if (guildKey != 0)
                {
                    m_app->Call_DeleteGuildMember(guildKey, user);
                }
                delete user;
                m_users.erase(it++);
                continue;
            }
        }
        ++it;
    }
}

CUser* CUserManager::FindUser(unsigned int dbid) const
{
    std::map<unsigned int, CUser*>::const_iterator it = m_users.find(dbid);
    return it == m_users.end() ? 0 : it->second;
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
        register char* mid = NumberToString(dbid, 0);
        DNF_LOG_SCOPE_AT(__FUNCTION__, 0x13c,"./log/LoginErr",
            "uDBID(%s) uCharNo(%d) is already exist at m_mapUsers!", mid, charNo);
    }
    user->SetUserPosState(2);
    if (charNo != 0)
    {
        if (InsertUser_CharNo(charNo, user) != 1)
        {
            register char* mid = NumberToString(dbid, 0);
            DNF_LOG_SCOPE_AT(__FUNCTION__, 0x146,"./log/LoginErr",
                "uDBID(%s) uCharNo(%d) is already exist at m_mapCharNoUsers!", mid, charNo);
        }
        if (InsertUser_CharName(charName, user) != 1)
        {
            register char* mid = NumberToString(dbid, 0);
            DNF_LOG_SCOPE_AT(__FUNCTION__, 0x14a,"./log/LoginErr",
                "uDBID(%s) uCharName(%s) is already exist at m_mapCharNameUsers!", mid, charName);
        }
        user->SetUserPosState(3);
    }
    return user;
}

bool CUserManager::InsertUser_CharNo(unsigned int charNo, CUser* user)
{
    if (user == 0)
    {
        return 0;
    }
    if (m_charNoUsers.insert(std::make_pair(charNo, user)).second)
    {
        return 1;
    }
    register char* name = user->GetCharName();
    register unsigned int dbid = user->GetDBID();
    register unsigned int nCharNo = charNo;
    CMyFileLog log(__FUNCTION__, 0x163);
    log("./log/Except",
        "[INSERT_ERR]Already Exist!\tChar No : %d\tDB No : %d\tChar_Name : %s\n",
        nCharNo, dbid, name);
    return 0;
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
    register int nSize = m_charNoUsers.size();
    register unsigned int nCharNo = charNo;
    CMyFileLog log(__FUNCTION__, 0x17e);
    log("./log/User",
        "[EXCEPT]CUserManager::DeleteUser_CharNo() : Erase Fail!\tChar No : %d\tChar_No Map Count : %d\n",
        nCharNo, nSize);
    return 0;
}

CUser* CUserManager::FindUser_CharNo(unsigned int charNo) const
{
    std::map<unsigned int, CUser*>::const_iterator it = m_charNoUsers.find(charNo);
    return it == m_charNoUsers.end() ? 0 : it->second;
}

bool CUserManager::InsertUser_CharName(char* name, CUser* user)
{
    if (user == 0)
    {
        return 0;
    }
    if (m_charNameUsers.insert(std::pair<const std::string, CUser*>(name, user)).second)
    {
        return 1;
    }
    register unsigned int dbid = user->GetDBID();
    register char* nName = name;
    CMyFileLog log(__FUNCTION__, 0x1a6);
    log("./log/Except", "[INSERT_ERR]Already Exist!\tChar Name : %s\tDB No : %d\n",
        nName, dbid);
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
    CMyFileLog log("DeleteUser_CharName", 0x1c1);
    log("./log/Except",
        "[EXCEPT]CUserManager::DeleteUser_CharNo() : Erase Fail!\tChar Name : %s\tChar_No Map Count : %d\n",
        name.c_str(), m_charNoUsers.size());
    return 0;
}

CUser* CUserManager::FindUser_CharName(std::string name) const
{
    std::map<std::string, CUser*>::const_iterator it = m_charNameUsers.find(name);
    return it == m_charNameUsers.end() ? 0 : it->second;
}

void CUserManager::ProcessByMinute()
{
    for (std::map<unsigned int, CUser*>::const_iterator it = m_charNoUsers.begin();
         it != m_charNoUsers.end(); ++it)
    {
        CUser* user = it->second;
        if (user != 0)
        {
            user->GuildInviteProcess();
        }
    }
}

void CUserManager::DeleteBlackUserOnCharacDelete(unsigned int charNo)
{
    if (m_charNoUsers.empty())
    {
        return;
    }
    for (std::map<unsigned int, CUser*>::const_iterator it = m_charNoUsers.begin();
         it != m_charNoUsers.end(); ++it)
    {
        CUser* user = it->second;
        user->DeleteToBlackList(charNo);
    }
}

void CUserManager::RefreshGuildAttendanceInfo()
{
    if (m_charNoUsers.empty())
    {
        return;
    }
    for (std::map<unsigned int, CUser*>::const_iterator it = m_charNoUsers.begin();
         it != m_charNoUsers.end(); ++it)
    {
        CUser* user = it->second;
        if (user->GetGuildKey() != 0)
        {
            m_app->Get_GuildManager()->AttendGuild(user->GetGuildKey(),
                                                   user->GetUniqCharNo());
        }
    }
}

int CUserManager::Size()
{
    return (int)m_users.size();
}
