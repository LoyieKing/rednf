#ifndef DBMW_DNFAPPLICATION_H_
#define DBMW_DNFAPPLICATION_H_

#include "DBMWCommon.h"


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

class CApplication : public CAppBase
{
public:
    CApplication();
    virtual ~CApplication();
    virtual void Process();
    virtual void Init(int argc, char** argv);
    virtual void Load(int argc, char** argv);
    virtual void Free();
    bool InitDB();
    bool OpenDB(ENUM_DB_HANDLE_IDX idx, std::string name);
    bool QueryConnInfo(ENUM_DB_HANDLE_IDX idx, ENUM_SERVER_GROUP serverGroup,
                       STDBConnInfo& connInfo);
    void CheckArgv(int argc, char** argv);
    void App_Stop();
    void SendTestPacket_1();
    void SendTestPacket_2();
    void TranslateSignal();
    void SwitchQueueTCP();
    void SwitchQueueUDP();
    int Send_Suspend_Signal(const std::string& msg);
    int Send_Term_Signal(const std::string& msg);
    void AttachAppInitor(char** argv);
    CServerHandler* Get_ServerHandler();
    void* Get_UdpHandler();
    CTcpNetSystem* Get_TcpNetSystem();
    CMutex* Get_UdpQLock();
    CMutex* Get_UdpBLock();
    CMutex* Get_QLock();
    CMutex* Get_BLock();
    UdpRecvQueue* Get_UdpPacketRecvQ();
    UdpRecvQueue* Get_UdpPacketParseQ();
    CTcpServer* FindTcpServer(unsigned int idx);
    CMonitorServer* FindMonitorServer(int idx);
    void* GetGMAccounts();
    CGameServer* FindGameServer(int idx);

    char m_loaded;                      // +4
    char m_pad5[3];                     // +5
    int m_field8;                       // +8
    CAppInit* m_appInitor;              // +0xc
    CAppConfig* m_appConfig;            // +0x10
    CServerConfig* m_serverConfig;      // +0x14
    CServerHandler* m_serverHandler;    // +0x18
    CFrameCountHandler m_frameCount;    // +0x1c
    CKillUSRConfig* m_killUsrConfig;    // +0x4c（原版为 CKillUSRConfig*）
    CDBManager m_dbManager;             // +0x50（0x48 字节，17 个 handle）
    CUdpHandler* m_udpHandler;          // +0x98
    CNetworkThread* m_networkThread;    // +0x9c
    CSwapQueue<UdpRecvQueue, 2> m_udpSwapQueue;  // +0xa0（0x58）
    CMutex m_mutexF8;                   // +0xf8
    CMutex m_mutex110;                  // +0x110
    CTcpNetSystem m_tcpNetSystem;       // +0x128（0x160）
    CGuildManager* m_guildManager;      // +0x288
    void* m_gmAccounts;                 // +0x28c（WongWork::CGMAccounts*）
};

#endif  // DBMW_DNFAPPLICATION_H_
