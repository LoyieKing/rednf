#ifndef MEMPOOL_H_
#define MEMPOOL_H_

#include <string>
#include <vector>

#include "PacketHeader.h"
#include "MemPool.h"


// from GuildMisc.h
template<class T>
class MemPool
{
public:
    MemPool(unsigned int count);
    ~MemPool();
    void* alloc();
    void free(void* p);
    void free(void* p, unsigned int size);
    static void* headOfFreeList_;
    int m_classSize;            // +0
    unsigned int m_count;       // +4
    std::vector<void*> m_chunks; // +8
};

class CUdpRecvBuffer;
class CBlackUser;
class CCashObject;
class CPacketBuffer;
class CTcpRecvBuffer;
class CTcpSendBuffer;
class STPowerWarGuildInfo;
class STPowerWarCharacInfo;
class CPeer;
class CUser;
class CGuild;
extern MemPool<CUdpRecvBuffer> m_RecvBufferMemPool_;
extern MemPool<CBlackUser> m_BlackUserMemPool_;
extern MemPool<CCashObject> m_CashObjectMemPool_;
extern MemPool<CPacketBuffer> m_PacketBufferMemPool_;
extern MemPool<CTcpRecvBuffer> m_TcpRecvBufferMemPool_;
extern MemPool<CTcpSendBuffer> m_TcpSendBufferMemPool_;
extern MemPool<STPowerWarGuildInfo> m_PowerWarGuildInfoMemPool_;
extern MemPool<STPowerWarCharacInfo> m_PowerWarCharacInfoMemPool_;
extern MemPool<CPeer> m_PeerMemPool_;
extern MemPool<CUser> m_UserMemPool_;
extern MemPool<CGuild> m_GuildMemPool_;

#endif
