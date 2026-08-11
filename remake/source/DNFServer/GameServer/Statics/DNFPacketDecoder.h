#ifndef DNF_PACKET_DECODER_H_
#define DNF_PACKET_DECODER_H_

#include <queue>

#include "PacketHeader.h"

class CApplication;
class CUdpRecvBuffer;

// ---- CPacketCounter<Lo,Hi>：包计数（MsgDecode 静态实例）----
template<int Lo, int Hi>
class CPacketCounter
{
public:
    CPacketCounter(char* name, char* title);
    ~CPacketCounter();
    void IncrementPacketCount(int id);
    void BeforeProcess();
    void AfterProcess(int id);
    void Reset();
};

// ---- CPacketDecoder：queue@0 / lock@4 / poolLock@8 / handlers@0xc（0x2800 项）----
class CPacketDecoder
{
public:
    CPacketDecoder();
    ~CPacketDecoder();
    void Attach(CApplication* app);
    bool MsgDecode(PacketHeader* pkt);
    void Process();
    void* m_queue;                 // +0
    void* m_lock;                  // +4
    void* m_poolLock;              // +8
    void (*m_handlers[0x2800])(PacketHeader*);   // +0xc
};

CPacketDecoder* CPacketDecoderInstance();

#endif // DNF_PACKET_DECODER_H_
