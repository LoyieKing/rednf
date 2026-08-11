#ifndef DNF_USER_H_
#define DNF_USER_H_

#include <map>

#include "PacketHeader.h"

class CApplication;
class CGameServer;

// Packet_DoubleCheck_UDP_Logout：PacketHeader 10 字节后为 userID
class Packet_DoubleCheck_UDP_Logout : public PacketHeader
{
public:
    unsigned int m_userID;  // +0xa
    unsigned char m_channel;  // +0xe
    unsigned char m_group;    // +0xf
} __attribute__((packed));

// CUser：CGameServer*@0（4）
class CUser
{
public:
    CUser();
    CUser(CGameServer* gs);
    ~CUser();
    CGameServer* GetGameServer();
    void SendToGameserver(char* buf, int len);
    CGameServer* m_gs;   // +0
};

// CUserManager：10 × map<const uint, CUser>@0（0x18）+ CApplication*@0xf0（0xf4）
class CUserManager
{
public:
    CUserManager();
    ~CUserManager();
    void Init(CApplication* app);
    bool DeleteUser(Packet_DoubleCheck_UDP_Logout* pkt);
    void CreateUser(unsigned int user_id, CGameServer* gs);
    bool FindUser(unsigned int user_id, CUser& out);
    std::map<const unsigned int, CUser> m_maps[10];  // +0
    CApplication* m_app;                              // +0xf0
};

#endif // DNF_USER_H_
