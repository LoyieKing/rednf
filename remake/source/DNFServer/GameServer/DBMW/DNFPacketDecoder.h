#ifndef DBMW_DNFPACKETDECODER_H_
#define DBMW_DNFPACKETDECODER_H_

#include "DBMWCommon.h"


class CPacketDecoder
{
public:
    CPacketDecoder();
    ~CPacketDecoder();
    void Process();
    void Attach(CApplication* app);
    void SetTCPQueue(TcpRecvQueue* q);
    void SetUdpQueue(UdpRecvQueue* q);
    void TcpProcess();
    void UdpProcess();
    bool MsgDecode(PacketHeader* header);
    UdpRecvQueue* m_udpQueue;    // +0
    CMutex* m_udpQLock;          // +4
    CMutex* m_udpBLock;          // +8
    TcpRecvQueue* m_tcpQueue;    // +0xc
    CMutex* m_tcpRecvQLock;      // +0x10
    CMutex* m_tcpRecvBLock;      // +0x14
    CServerHandler* m_serverHandler;  // +0x18
    void (*m_table[0x2418])(PacketHeader*);  // +0x1c（0x3e8..0x27ff）
};

#endif  // DBMW_DNFPACKETDECODER_H_
