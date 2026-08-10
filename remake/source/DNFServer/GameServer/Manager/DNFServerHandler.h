#ifndef DNF_SERVER_HANDLER_H_
#define DNF_SERVER_HANDLER_H_

#include <map>

#include "DNFMonitorServer.h"
#include "DNFServerConfig.h"
#include "PacketHeader.h"

class CApplication;
class CTcpServer;

// ---- CServerHandler（最小：+0x7e4 m_app）----
class CServerHandler
{
public:
    CServerHandler();
    ~CServerHandler();
    void Attach(CApplication* app);
    void SetMonitorServerIpPort(unsigned char idx, unsigned int ip, unsigned short port);
    void SendToMonitorServer(char* buf, int len, unsigned char idx);
    void SendToTcpServer(PacketHeader* header, unsigned char idx);
    void SendToTcpServer(char* buf, int len, unsigned char idx);
    void SendAllTcpServer(PacketHeader* header);
    void SendAllToMonitorServer(char* buf, int len);
    char CreateTcpServer(unsigned char idx, unsigned int port);
    char DeleteTcpServer(unsigned char idx);
    void Process();
    void Load(ST_ServerInfo* info);
    CTcpServer* GetTcpServer(unsigned int idx);
    CTcpServer* GetTcpServer(unsigned char idx);
    CMonitorServer* GetMonitorServer(int idx);
    int GetAlivedMonitorServer();
    void ResetHeartBeat(unsigned char idx);
    char IsConnectedMonitorServer(unsigned char idx);
    void CheckTcpServerHeartbeat();
    void SetConnectFlag(unsigned char idx, bool flag);
    CMonitorServer m_monitorServers[0x65];  // +0（101 × 0x14 = 0x7e4）
    CApplication* m_app;  // +0x7e4
    std::map<unsigned int, CTcpServer*> m_tcpServers;  // +0x7e8
    char m_pad[4];        // +0x800（sizeof=0x804）
};

#endif  // DNF_SERVER_HANDLER_H_
