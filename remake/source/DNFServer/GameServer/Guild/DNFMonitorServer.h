#ifndef DNFMONITORSERVER_H_
#define DNFMONITORSERVER_H_

#include <map>
#include <string>

#include "PacketHeader.h"
#include "DNFServerInterface.h"

class CMonitorServer;
class stServerInfo;

// from GuildServer.h
class CMonitorServer : public CServerInterface
{
public:
    CMonitorServer();
    CMonitorServer(stServerInfo* info);
    ~CMonitorServer();
    bool Initialize();
    bool Destroy();
    void SendHeartBeat(int group);
};

#endif
