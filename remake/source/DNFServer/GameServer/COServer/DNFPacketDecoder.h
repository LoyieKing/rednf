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
    // ORIG 实测（0x8056e32）：间接调用为 typed 函数指针数组形态
    // （mov 0x4(%eax,%edx,4),%edx; call *%edx），与 void*+强转形态
    // （多一条 mov %eax,%edx）不同。
    typedef void (*PacketHandlerFn)(PacketHeader*);
    PacketHandlerFn m_handlers[0x27fd];  // +4
};

CPacketDecoder* CPacketDecoderInstance();

#endif // DNF_PACKET_DECODER_H_
