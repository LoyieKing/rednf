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
    bool m_field8;           // +8
    char m_field9;           // +9
    char m_fielda;           // +0xa
    int m_sock;              // +0xc
};

#endif
