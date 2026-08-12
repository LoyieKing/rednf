#ifndef DNFAPPLICATION_H_
#define DNFAPPLICATION_H_

#include <deque>
#include <map>
#include <queue>
#include <string>
#include "Thread.h"
#include "PacketHeader.h"
#include "DNFGuild.h"

#include "PacketHeader.h"
#include "DNFApplication.h"
#include "DNFGuildManager.h"
#include "DNFTickHandler.h"
#include "DNFUserManager.h"
#include "GMAccounts.h"
#include "PowerManager.h"
#include "TcpNetSystem.h"

class CAppBase;
class CAppConfig;
class CAppInit;
class CApplication;
class CGMAccounts;
class CGameServer;
class CInnerMsgHandler;
class CKillUSRConfig;
class CMemoryCashManager;
class CServerConfig;
class CServerHandler;
class CTcpGameServer;
class CUdpHandler;
class CUdpNetworkThread;
class CUdpRecvBuffer;
class CUser;
struct Packet_DB_Query_On_Guild_Booting;
struct Packet_Guild_Master_Delegate_FromWeb;
struct Packet_Monitor_Event_End;
struct Packet_Monitor_Event_Start;
struct Packet_Monitor_Notice_Guild_Enter;
struct Packet_Monitor_Send_Guild_Mail;
struct Packet_Monitor_Set_GuildMember_Grade_FromWeb;

// from GuildApp.h
class CAppBase
{
public:
    CAppBase();
    virtual ~CAppBase();
    virtual void Process();
    virtual void Init(int argc, char** argv) = 0;
    virtual void Load(int argc, char** argv) = 0;
    virtual void Free() = 0;
    void Create(int argc, char** argv);
    void Clear();
};

// from GuildApp.h
class CApplication : public CAppBase
{
public:
    CApplication();
    virtual ~CApplication();
    virtual void Process();
    virtual void Init(int argc, char** argv);
    virtual void Load(int argc, char** argv);
    virtual void Free();
    void CheckArgv(int argc, char** argv);
    int Send_Term_Signal(const std::string& name);
    void Send_Suspend_Signal(const std::string& name);
    void App_Stop();
    void SendTestPacket_1();
    void SendTestPacket_2();
    void TranslateSignal();
    void AttachAppInitor(char** argv);
    unsigned char Get_ServerGroup();
    CServerHandler* Get_ServerHandler();
    CUdpHandler* Get_UdpHandler();
    CMutex* Get_UdpQLock();
    CMutex* Get_UdpBLock();
    void* Get_UdpPacketRecvQ();
    void* Get_UdpPacketParseQ();
    void SwitchQueueTCP();
    void SwitchQueueUDP();
    CTcpNetSystem* Get_TcpNetSystem();
    CGuildManager* Get_GuildManager();
    CPowerManager* GetPowerManager();
    CUserManager* Get_UserManager();
    CMemoryCashManager* Get_MemoryCashManager();
    CGameServer* FindGameServer(int group);
    WongWork::CGMAccounts* GetGMAccounts();
    CTcpGameServer* FindTcpGameServer(unsigned int group);
    void OnGameServerDown(CGameServer* server);
    void OnTcpGameServerDown(CTcpGameServer* server);
    void Call_DeleteGuildMember(unsigned int group, CUser* user);

    bool m_loaded;                      // +4
    int m_reserved8;                    // +8
    CAppInit* m_appInit;                // +0xc
    CUserManager m_userManager;         // +0x10
    CAppConfig* m_appConfig;            // +0x5c
    CServerConfig* m_serverConfig;      // +0x60
    CKillUSRConfig* m_killConfig;       // +0x64
    CServerHandler* m_serverHandler;    // +0x68
    CFrameCountHandler m_frameCount;    // +0x6c
    CInnerMsgHandler* m_innerMsgHandler;  // +0x9c
    CSwapQueue<std::queue<CUdpRecvBuffer*, std::deque<CUdpRecvBuffer*> >, 2> m_swapQueue;  // +0xa0
    CUdpHandler* m_udpHandler;          // +0xf8
    CUdpNetworkThread* m_udpThread;     // +0xfc
    CMutex m_udpQLock;                  // +0x100
    CMutex m_udpBLock;                  // +0x118
    CTcpNetSystem m_tcpNetSystem;       // +0x130
    CGuildManager m_guildManager;       // +0x290
    CPowerManager m_powerManager;       // +0x370
    WongWork::CGMAccounts* m_gmAccounts; // +0x50c
    CMemoryCashManager* m_memoryCash;   // +0x510
};

// from GuildPackets.h
class Packet_DB_Query_On_Guild_Booting : public PacketHeader {
public:
    Packet_DB_Query_On_Guild_Booting();
    char m_data[0x1];
};

// from GuildPackets.h
class Packet_Guild_Master_Delegate_FromWeb : public PacketHeader {
public:
    Packet_Guild_Master_Delegate_FromWeb();
    char m_data[0x2a];
};

// from GuildPackets.h
class Packet_Monitor_Event_End : public PacketHeader {
public:
    Packet_Monitor_Event_End();
    unsigned int m_fieldA;      // +0xa
} __attribute__((packed));

// from GuildPackets.h
class Packet_Monitor_Event_Start : public PacketHeader {
public:
    Packet_Monitor_Event_Start();
    unsigned int m_fieldA;      // +0xa
    char m_data[0x4];
} __attribute__((packed));

// from GuildPackets.h
#pragma pack(push,1)
class Packet_Monitor_Notice_Guild_Enter : public PacketHeader {
public:
    Packet_Monitor_Notice_Guild_Enter();
    ST_Notice_Guild_Enter m_info;  // +0xa
    char m_pad[5];                 // +0x4b
};
#pragma pack(pop)

// from GuildPackets.h
class Packet_Monitor_Send_Guild_Mail : public PacketHeader {
public:
    Packet_Monitor_Send_Guild_Mail();
    char m_data[0x108];
};

// from GuildPackets.h
class Packet_Monitor_Set_GuildMember_Grade_FromWeb : public PacketHeader {
public:
    Packet_Monitor_Set_GuildMember_Grade_FromWeb();
    char m_data[0xd];
};

void ShowLogo();
CApplication* CApplicationInstance();

#endif
