#ifndef DBMW_DNFSTATISTICSSERVER_H_
#define DBMW_DNFSTATISTICSSERVER_H_

#include "DBMWCommon.h"


class CStatisticsServer
{
public:
    CStatisticsServer();
    ~CStatisticsServer();
    void Init(unsigned char type, std::string& name, unsigned short port,
              unsigned char flag);
    void SendToServer(char* buf, int len);
    void SendHeartBeat();
    char IsValidGameServer();
    void OnDisconnect();
    unsigned char m_type;  // +0
    char m_pad1[3];
    std::string m_name;  // +4
    unsigned short m_port;  // +8
    char m_padA;         // +0xa
    char m_flag;         // +0xb
    CUdpHandler* m_udpHandler;  // +0xc
};

#endif  // DBMW_DNFSTATISTICSSERVER_H_
