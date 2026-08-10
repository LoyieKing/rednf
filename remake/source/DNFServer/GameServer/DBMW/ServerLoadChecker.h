#ifndef DBMW_SERVERLOADCHECKER_H_
#define DBMW_SERVERLOADCHECKER_H_

#include "DBMWCommon.h"


class CAppLoadChecker
{
public:
    CAppLoadChecker();
    bool CheckTcpRecvQ(int size);
    bool CheckUdpRecvQ(int size);
    bool CheckTcpSendQ(int size);
    bool checkTcpRecvLoad(int size);
    bool checkUdpRecvLoad(int size);
    bool checkTcpSendLoad(int size);
    void setTcpRecvQueue(int size);
    void setUdpRecvQueue(int size);
    void setTcpSendQueue(int size);
    void RequestDB(CServerHandler* serverHandler, int flag, int size);
    int m_tcpRecvLast;   // +0
    int m_udpRecvLast;   // +4
    int m_tcpSendLast;   // +8
    char m_tcpRecvLevel; // +0xc
    char m_udpRecvLevel; // +0xd
    char m_tcpSendLevel; // +0xe
    char m_padF;         // +0xf
};

#endif  // DBMW_SERVERLOADCHECKER_H_
