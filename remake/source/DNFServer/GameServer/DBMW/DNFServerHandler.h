#ifndef DBMW_DNFSERVERHANDLER_H_
#define DBMW_DNFSERVERHANDLER_H_

#include "DBMWCommon.h"


class CServerHandler
{
public:
    CServerHandler();
    ~CServerHandler();
    void Attach(CApplication* app);
    void Load(ST_ServerInfo* info);
    void Process();
    CTcpServer* GetTcpServer(unsigned char idx);
    CTcpServer* GetTcpServer(unsigned int socket);
    CGameServer* GetGameServer(int idx);
    CGuildServer* GetGuildServer();
    CMonitorServer* GetMonitorServer();
    CStatisticsServer* GetStatisticsServerPtr();
    char CreateTcpServer(unsigned char idx, unsigned int port);
    bool DeleteTcpServer(unsigned char idx);
    void SendAllTcpServer(PacketHeader* header);
    void SendAllToGameServer(char* buf, int len);
    void CheckTcpServerHeartbeat();
    // 兼容旧 API（manager 遗留 CPacketTranslater handler 使用；待其按 dbmw 重做后清理）
    int GetAlivedMonitorServer();
    void ResetHeartBeat(unsigned char idx);
    char IsConnectedMonitorServer(unsigned char idx);
    void SetConnectFlag(unsigned char idx, bool flag);
    void SendToTcpServer(PacketHeader* header, unsigned char idx);
    void SendToTcpServer(char* buf, int len, unsigned char idx);
    void SendAllToMonitorServer(char* buf, int len);
    CGameServer m_gameServers[0xff];    // +0（255 × 0x10 = 0xff0）
    CMonitorServer m_monitorServer;     // +0xff0
    std::map<unsigned char, CTcpServer*> m_tcpServers;  // +0x1000
    CGuildServer m_guildServer;         // +0x1018
    CApplication* m_app;                // +0x1028
    int m_tickCount;                    // +0x102c
    CStatisticsServer m_statisticsServer;  // +0x1030
};

#endif  // DBMW_DNFSERVERHANDLER_H_
