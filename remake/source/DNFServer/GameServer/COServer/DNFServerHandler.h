#ifndef DNF_SERVER_HANDLER_H_
#define DNF_SERVER_HANDLER_H_

#include "DNFGameServer.h"
#include "DNFServerConfig.h"

class CApplication;
class PacketHeader;

// CServerHandler：CGameServer[0x649b] + app@0x7dc1c，总 0x7dc20
// 数组下标 = group * 0xff + index（group<0x65, index<0xff）
class CServerHandler
{
public:
    CServerHandler();
    ~CServerHandler();
    void Attach(CApplication* app);
    void Load(ST_ServerInfo* info);
    void Process();
    void ResetHeartBeat(unsigned char group, unsigned char index);
    void SetGameServerIpPort(unsigned char id, unsigned int ip, unsigned short port);
    bool IsConnectedGameServer(unsigned char group, unsigned char index);
    void SetConnectFlag(unsigned char group, unsigned char index, bool flag);
    CGameServer* GetGameServer(int idx);
    void SendToGameServer(unsigned char group, unsigned char index, PacketHeader* header);
    void SendAllToGameServer(char* buf, int len);
    CGameServer m_servers[0x649b];  // +0
    CApplication* m_app;            // +0x7dc1c
};

#endif // DNF_SERVER_HANDLER_H_
