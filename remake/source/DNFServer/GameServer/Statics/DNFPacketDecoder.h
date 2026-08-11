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
    // 布局（ORIG 二进制实测）：+0 count / +4 t / +8 counts[0x2418] /
    // +0x9068 snapshot[0x2418] / +0x120c8 pending[0x2418] / +0x144e0 diffs[0x2418] /
    // +0x1d540 path[0x100] / +0x1d640 bProcess / +0x1d641 bInit
    unsigned int m_count;                 // +0
    time_t m_t;                           // +4
    unsigned int m_counts[0x2418];        // +8
    unsigned int m_snapshot[0x2418];      // +0x9068
    unsigned char m_pending[0x2418];      // +0x120c8
    unsigned int m_diffs[0x2418];         // +0x144e0
    char m_path[0x100];                   // +0x1d540
    bool m_bProcess;                      // +0x1d640
    unsigned char m_bInit;                // +0x1d641
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
