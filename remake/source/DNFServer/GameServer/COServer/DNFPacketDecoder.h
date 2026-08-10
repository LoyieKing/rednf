#ifndef DNF_PACKET_DECODER_H_
#define DNF_PACKET_DECODER_H_

#include <queue>

#include "PacketHeader.h"

class CApplication;
class CUdpRecvBuffer;
class CMutex;

// ---- CPacketDecoder：lock@0 + handlers[0x27fd]@4 ----
class CPacketDecoder
{
public:
    CPacketDecoder();
    ~CPacketDecoder();
    void Attach(CApplication* app);
    int MsgDecode(PacketHeader* pkt);
    void Process(std::queue<CUdpRecvBuffer*>* q, CMutex* lock);
    void* m_poolLock;              // +0
    void* m_handlers[0x27fd];      // +4
};

CPacketDecoder* CPacketDecoderInstance();

#endif // DNF_PACKET_DECODER_H_
