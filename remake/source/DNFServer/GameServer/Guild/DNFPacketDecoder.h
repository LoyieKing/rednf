#ifndef DNFPACKETDECODER_H_
#define DNFPACKETDECODER_H_

#include <queue>
#include <string>
#include "PacketHeader.h"
#include "Thread.h"

#include "PacketHeader.h"

class CApplication;
class CPacketDecoder;
class CTcpRecvBuffer;
class CUdpRecvBuffer;

// from GuildPacket.h
class CPacketDecoder
{
public:
    CPacketDecoder();
    ~CPacketDecoder();
    void Attach(CApplication* app);
    void Process();
    void TcpProcess();
    void UdpProcess();
    bool MsgDecode(PacketHeader* pkt);
    void SetTCPQueue(std::queue<CTcpRecvBuffer*>* q);
    void SetUdpQueue(std::queue<CUdpRecvBuffer*>* q);
    void* m_udpParseQ;                          // +0
    void* m_udpQLock;                           // +4
    void* m_udpBLock;                           // +8
    union {
        struct {
            void* m_tcpParseQ;                  // +0xc
            void* m_tcpRecvQLock;               // +0x10
            void* m_tcpRecvBLock;               // +0x14
            void* m_serverHandler;              // +0x18
        };
        void (*m_table[0x2804])(PacketHeader*); // +0xc（handler 表，按 ORIG 0xa01c 布局）
    };
};

CPacketDecoder* CPacketDecoderInstance();

#endif
