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
class CServerHandler;

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
    CMutex* m_udpQLock;                         // +4
    CMutex* m_udpBLock;                         // +8
    union {
        struct {
            void* m_tcpParseQ;                  // +0xc
            CMutex* m_tcpRecvQLock;             // +0x10
            CMutex* m_tcpRecvBLock;             // +0x14
            // [2026-09-15 修复/P301] 原为 void* + 调用点 cast ⇒ 4.4 生两段装载
            //   （`mov 0x18(%eax),%eax; mov %eax,%ebx`）；ORIG 0x8070?? TcpProcess/UdpProcess
            //   为单段 `mov 0x18(%eax),%ebx`（成员具型化为 CServerHandler*）。sizeof/偏移不变。
            CServerHandler* m_serverHandler;    // +0x18
        };
        void (*m_table[0x2804][1][1])(PacketHeader*); // +0xc（handler 表，按 ORIG 0xa01c 布局）
    };
};

CPacketDecoder* CPacketDecoderInstance();

#endif
