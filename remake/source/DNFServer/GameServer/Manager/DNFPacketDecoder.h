#ifndef DNF_PACKET_DECODER_H_
#define DNF_PACKET_DECODER_H_

#include "DNFPacketBuffer.h"
#include "PacketHeader.h"

class CApplication;
class CMutex;

// ---- CPacketDecoder ----
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
    char MsgDecode(PacketHeader* header);
    TcpRecvQueue* m_tcpQueue;  // +0
    CMutex* m_tcpRecvQLock;    // +4
    CMutex* m_tcpRecvBLock;    // +8
    UdpRecvQueue* m_udpQueue;  // +0xc
    CMutex* m_udpQLock;        // +0x10
    CMutex* m_udpBLock;        // +0x14
    char m_table[0xa018 - 0x18];  // +0x18（0x3e8..0x27ff 表项）
};

CPacketDecoder* CPacketDecoderInstance();

#endif  // DNF_PACKET_DECODER_H_
