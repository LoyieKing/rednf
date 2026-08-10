#ifndef SERVERLOADCHECKER_H_
#define SERVERLOADCHECKER_H_

#include <map>
#include <string>
#include <vector>

#include "PacketHeader.h"

class CAppLoadChecker;
class CServerHandler;

// from GuildTable.h
class CAppLoadChecker
{
public:
    CAppLoadChecker();
    void AddLoad(int n);
    void AddLoadTotal(int n);
    int IsLoadComplete();
    void setUdpRecvQueue(int n);
    void setTcpRecvQueue(int n);
    void setTcpSendQueue(int n);
    bool checkUdpRecvLoad(int n);
    bool checkTcpRecvLoad(int n);
    bool checkTcpSendLoad(int n);
    int CheckUdpRecvQ(int n);
    int CheckTcpRecvQ(int n);
    int CheckTcpSendQ(int n);
    void RequestDB(CServerHandler* handler, int a, int b);
    int m_tcpRecvLast;   // +0
    int m_udpRecvLast;   // +4
    int m_tcpSendLast;   // +8
    char m_tcpRecvLevel; // +0xc
    char m_udpRecvLevel; // +0xd
    char m_tcpSendLevel; // +0xe
    char m_padF;         // +0xf
};

CAppLoadChecker* CAppLoadCheckerInstance();

#endif
