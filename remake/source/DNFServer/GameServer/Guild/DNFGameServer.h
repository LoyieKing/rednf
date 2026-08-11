#ifndef DNFGAMESERVER_H_
#define DNFGAMESERVER_H_

#include <map>
#include <string>

#include "PacketHeader.h"
#include "DNFServerInterface.h"

class CGameServer;
class CTcpGameServer;
class CTcpNetSystem;
class stServerInfo;

// from GuildServer.h
class CGameServer : public CServerInterface
{
public:
    CGameServer();
    CGameServer(stServerInfo* info);
    ~CGameServer();
    bool Initialize();
    bool Destroy();
    int GetSocket();
    void SetSocket(unsigned int sock);
    unsigned int m_field10; // +0x10
};

// from GuildServer.h
class CTcpGameServer
{
public:
    CTcpGameServer();
    ~CTcpGameServer();
    void Init(unsigned int group, CTcpNetSystem* net);
    void SendToGameServer(char* buf);
    unsigned char GetChannelNo();
    void SetChannelNo(unsigned char channel);
    bool IsValidServer();
    char* makePacketHeader(unsigned short id, unsigned short size);
    unsigned int m_group;         // +0
    CTcpNetSystem* m_net;         // +4
    unsigned char m_channel;      // +8
};

#endif
