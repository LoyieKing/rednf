#ifndef DNFDBSERVER_H_
#define DNFDBSERVER_H_

#include <map>
#include <string>

#include "PacketHeader.h"
#include "DNFServerInterface.h"

class CDBServer;
class CGuildManager;
class CTcpDBServer;
class CTcpNetSystem;
class stServerInfo;

// from GuildServer.h
class CDBServer : public CServerInterface
{
public:
    CDBServer();
    CDBServer(stServerInfo* info);
    ~CDBServer();
    bool Initialize();
    bool Destroy();
};

// from GuildServer.h
class CTcpDBServer
{
public:
    CTcpDBServer();
    ~CTcpDBServer();
    void Init(CTcpNetSystem* net, CGuildManager* gm);
    void SetIP(std::string ip);
    void SetPort(unsigned short port);
    void SendHeartbeat();
    void SendToServer(char* buf);
    bool IsValidServer();
    char* GetIP();
    unsigned short GetPort();
    int* GetSockRef();
    int GetSock();
    void Clear();
    void SendLogin();
    void SendLogout();
    void Connected();
    void DisConnected();
    char* makePacketHeader(unsigned short id, unsigned short size);
    std::string m_ip;             // +0
    unsigned short m_port;        // +4
    int m_sock;                   // +8
    CTcpNetSystem* m_net;         // +0xc
    CGuildManager* m_guildMgr;    // +0x10
};

#endif
