#ifndef DNFSERVERHANDLER_H_
#define DNFSERVERHANDLER_H_

#include "PacketHeader.h"
#include <map>
#include <string>

#include "PacketHeader.h"
#include "DNFDBServer.h"

class CApplication;
class CDBServer;
class CGameServer;
class CManagerServer;
class CMonitorServer;
class CServerHandler;
class CTcpGameServer;
struct Packet_DBMW_Connection_Check;
struct Packet_DB_Query_Guild;
struct Packet_DB_Query_Guild_Member;
class stServerInfo;

// from GuildPackets.h
class Packet_DBMW_Connection_Check : public PacketHeader {
public:
    Packet_DBMW_Connection_Check();
    char m_data[0x1];
};

// from GuildPackets.h
class Packet_DB_Query_Guild : public PacketHeader {
public:
    Packet_DB_Query_Guild();
    unsigned char m_data;     // +0xa
    unsigned int m_group;     // +0xb
    unsigned int m_guildId;   // +0xf
} __attribute__((packed));

// from GuildPackets.h
class Packet_DB_Query_Guild_Member : public PacketHeader {
public:
    Packet_DB_Query_Guild_Member();
    unsigned char m_group;     // +0xa
    unsigned int m_characNo;   // +0xb
} __attribute__((packed));

// from GuildServer.h
class CServerHandler
{
public:
    CServerHandler();
    ~CServerHandler();
    void Load(std::multimap<unsigned int, stServerInfo*>* map);
    void Attach(CApplication* app);
    void Process();
    CGameServer* GetGameServer(unsigned int group);
    CTcpGameServer* GetTcpGameServer(unsigned int group);
    bool IsConnectedGameServer(unsigned char group);
    CDBServer* GetDBServer();
    CManagerServer* GetManagerServer();
    CMonitorServer* GetMonitorServer();
    CTcpDBServer* GetTcpDBServer();
    bool IsConnectedDBServer();
    void ResetDBHeartBeat();
    void ResetHeartBeat(unsigned char type);
    void SetConnectFlag(unsigned char type, bool flag);
    void SetDBConnectFlag(bool flag);
    void SetManagerConnectFlag(bool flag);
    unsigned char GetServerGroupNo();
    void SendToDB(PacketHeader* pkt);
    void TcpSendToDB(PacketHeader* pkt);
    void SendToManager(PacketHeader* pkt);
    void SendToGameServer(unsigned char group, PacketHeader* pkt);
    void SendAllTcpGameServer(PacketHeader* pkt);
    void SendAllUdpGameServer(char* buf, int len);
    void SendTcpGameServerFirst(PacketHeader* pkt);
    bool RegistDBServer(CDBServer* server);
    bool UnregistDBServer();
    bool RegistManagerServer(CManagerServer* server);
    bool UnregistManagerServer();
    bool RegistMonitorServer(CMonitorServer* server);
    bool UnregistMonitorServer();
    bool RegistGameServer(stServerInfo* info);
    bool UnregistGameServer(unsigned int group);
    CTcpGameServer* CreateTcpGameServer(unsigned int group);
    bool DeleteTcpGameServer(unsigned int group);
    void SetGameServerIpPort(unsigned char group, unsigned int port, unsigned short tcpPort);
    void QueryGuild(unsigned int group, unsigned int guildId);
    void QueryGuildMember(unsigned char group, unsigned int characNo);
    void SendDBMWConnectionCheck();

    std::map<unsigned int, CGameServer*> m_gameServers;   // +0
    CDBServer* m_dbServer;                                 // +0x18
    CManagerServer* m_managerServer;                       // +0x1c
    CMonitorServer* m_monitorServer;                       // +0x20
    std::map<unsigned int, CTcpGameServer*> m_tcpGameServers;  // +0x24
    CApplication* m_app;                                   // +0x3c
    int m_heartbeat;                                       // +0x40
    CTcpDBServer m_tcpDbServer;                            // +0x44
    int m_hbCnt;                                           // +0x58（心跳计数，>3 发送）
};

#endif
