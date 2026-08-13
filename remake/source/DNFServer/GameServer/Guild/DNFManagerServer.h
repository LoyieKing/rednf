#ifndef DNFMANAGERSERVER_H_
#define DNFMANAGERSERVER_H_

#include "PacketHeader.h"
#include <map>
#include <string>

#include "PacketHeader.h"
#include "DNFServerInterface.h"

class CManagerServer;
struct Packet_Monitor_UDP_HeartBeat;
class stServerInfo;

// from GuildPackets.h
class Packet_Monitor_UDP_HeartBeat : public PacketHeader {
public:
    Packet_Monitor_UDP_HeartBeat();
    unsigned char m_group;   // +0xa（发送时写 group）
};

// from GuildServer.h
class CManagerServer : public CServerInterface
{
public:
    CManagerServer();
    CManagerServer(stServerInfo* info);
    ~CManagerServer();
    bool Initialize();
    bool Destroy();
    void SendHeartBeat(int group);
};

#endif
