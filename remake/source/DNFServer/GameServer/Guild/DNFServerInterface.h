#ifndef DNFSERVERINTERFACE_H_
#define DNFSERVERINTERFACE_H_

#include <map>
#include <string>

#include "PacketHeader.h"

class CServerInterface;
class stServerInfo;

// from GuildServer.h
class CServerInterface
{
public:
    CServerInterface();
    CServerInterface(stServerInfo* info);
    ~CServerInterface();
    bool IsValidServer();
    bool IsConnected();
    int IsHeartBeatTimeOver();
    void ResetHeartBeat();
    void OnDisconnect();
    int SendToServer(char* buf, int len);
    void SetConnFlag(bool flag);
    virtual bool Initialize();
    virtual bool Destroy();
    void SetServerInfo(stServerInfo* info);
    stServerInfo* GetServerInfo();
    void* GetUdpHandler();
    unsigned char GetChannelNo();
    unsigned char GetGroupNo();
    stServerInfo* m_info;    // +4
    bool m_connected;        // +8（IsConnected/SetConnFlag）
    char m_heartBeatCnt;     // +9（心跳倒计时 0x14）
    char m_heartBeatOverCnt; // +0xa（超时累计）
    int m_sock;              // +0xc
};

#endif
