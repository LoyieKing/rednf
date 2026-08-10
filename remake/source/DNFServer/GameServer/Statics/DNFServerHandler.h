#ifndef DNF_SERVER_HANDLER_H_
#define DNF_SERVER_HANDLER_H_

#include <string>

#include "DNFGameServer.h"
#include "DNFDBServer.h"
#include "DNFManagerServer.h"
#include "DNFServerConfig.h"
#include "PacketHeader.h"

class CApplication;

// global_function（statics 引用 global_function::SendPacketToDbmw）
namespace global_function
{
void SendPacketToDbmw(char* data);
}

// CServerHandler：CGameServer[0xff]@0 + CDBServer@0x13ec + @0x1400 +
//                CManagerServer@0x1404 + @0x1414（总 0x1418）
class CServerHandler
{
public:
    CServerHandler();
    ~CServerHandler();
    void Attach(CApplication* app);
    void Load(ST_ServerInfo* info);
    void Process();
    void ResetHeartBeat(unsigned char index);
    char IsConnectedGameServer(unsigned char index);
    void SetConnectFlag(unsigned char index, bool flag);
    CGameServer* GetGameServer(int idx);
    void SendToGameServer(unsigned char index, PacketHeader* header);
    void SendAllToGameServer(char* buf, int len);
    void SendToDB(PacketHeader* header);
    void SendToManager(PacketHeader* header);
    void SetDBConnectFlag(bool flag);
    void SetManagerConnectFlag(bool flag);
    void ResetDBHeartBeat();
    char IsConnectedDBServer();
    unsigned char GetServerGroupNo();
    void SetGameServerIpPort(unsigned char id, unsigned int ip, unsigned short port);
    CGameServer m_servers[0xff];  // +0
    CDBServer m_dbServer;         // +0x13ec
    CApplication* m_app;          // +0x1400
    CManagerServer m_mgrServer;   // +0x1404
    int m_reserved2;              // +0x1414（总 0x1418）
};

#endif // DNF_SERVER_HANDLER_H_
