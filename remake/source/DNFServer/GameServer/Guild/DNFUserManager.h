#ifndef DNFUSERMANAGER_H_
#define DNFUSERMANAGER_H_

#include <map>
#include <queue>
#include <deque>
#include <list>
#include <algorithm>
#include <string>
#include <vector>
#include "Thread.h"

#include "PacketHeader.h"

class CApplication;
class CGameServer;
class CTcpGameServer;
class CUser;
class CUserManager;

// from GuildDomain.h
class CUserManager
{
public:
    CUserManager();
    ~CUserManager();
    void Init(CApplication* app);
    void Process();
    void ProcessByMinute();
    int Size();
    CUser* CreateUser(unsigned int dbid, unsigned int charNo, char* charName, int channel,
                      CGameServer* server);
    void DeleteUser(unsigned int dbid);
    int DeleteUser(CUser* user);
    int DeleteUser_CharNo(unsigned int charNo);
    int DeleteUser_CharName(std::string name);
    bool InsertUser(unsigned int dbid, CUser* user);
    bool InsertUser_CharNo(unsigned int charNo, CUser* user);
    bool InsertUser_CharName(char* name, CUser* user);
    CUser* FindUser(unsigned int dbid) const;
    CUser* FindUser_CharNo(unsigned int charNo) const;
    CUser* FindUser_CharName(std::string name) const;
    void DeleteUsersOnGameServerDown(CGameServer* server);
    void DeleteUsersOnTcpGameServerDown(CTcpGameServer* server);
    void DeleteBlackUserOnCharacDelete(unsigned int charNo);
    void RefreshGuildAttendanceInfo();
    std::map<const unsigned int, CUser*> m_users;         // +0
    std::map<const unsigned int, CUser*> m_charNoUsers;   // +0x18
    std::map<const std::string, CUser*> m_charNameUsers;  // +0x30
    CApplication* m_app;                            // +0x48
};

#endif
