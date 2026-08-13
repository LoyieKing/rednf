#ifndef DBMW_DNFPACKETBUFFER_H_
#define DBMW_DNFPACKETBUFFER_H_

#include "DBMWCommon.h"

struct __attribute__((packed)) DBmwPacketBufferHeader
{
    unsigned short packetId;   // +0
    unsigned short packetSize; // +2
    unsigned short reversed1;  // +4
    unsigned int reversed2;    // +6
};

class CUdpRecvBuffer
{
public:
    static void* operator new(unsigned int size);
    static void operator delete(void* ptr);
    static void operator delete(void* ptr, unsigned int size);
    DBmwPacketBufferHeader m_header;  // +0
    char m_payload[0x17fa];           // +0xa
};

class CTcpRecvBuffer
{
public:
    static void* operator new(unsigned int size);
    static void operator delete(void* ptr);
    static void operator delete(void* ptr, unsigned int size);
    DBmwPacketBufferHeader m_header;  // +0
    char m_payload[0x17fa];           // +0xa
};

class CTcpSendBuffer
{
public:
    static void* operator new(unsigned int size);
    static void operator delete(void* ptr);
    static void operator delete(void* ptr, unsigned int size);
    DBmwPacketBufferHeader m_header;  // +0
    char m_payload[0x17fa];           // +0xa
};

class CPacketBuffer
{
public:
    static void* operator new(unsigned int size);
    static void operator delete(void* ptr);
    static void operator delete(void* ptr, unsigned int size);
    DBmwPacketBufferHeader m_header;  // +0
    char m_payload[0x17fa];           // +0xa
};

template<class T>
class MemPool
{
public:
    MemPool();
    MemPool(unsigned int count);
    ~MemPool();
    void* alloc();
    void free(void* ptr);
    void free(void* ptr, unsigned int size);
    static void* headOfFreeList_;
    int m_size;                     // +0
    unsigned int m_count;           // +4（ORIG 为 unsigned；影响 ctor 求值顺序）
    std::vector<void*> m_blocks;    // +8
};

template<class T>
class IQueue
{
public:
    IQueue() {}
    ~IQueue() {}
    static IQueue& Get();
    char InitQueue(T* recv, T* parse) { m_recv = recv; m_parse = parse; return 1; }
    T* GetParseQueue() { return m_parse; }
    bool SwitchQueue();
    T* m_recv;   // +0
    T* m_parse;  // +4
};

template<class T, int N>
class CSwapQueue
{
public:
    CSwapQueue() { Init(); }
    ~CSwapQueue() {}
    void Init() { m_recvIdx = 0; m_parseIdx = 1; }
    T* GetRecvQ() { return &m_queues[m_recvIdx]; }
    T* GetParseQ() { return &m_queues[m_parseIdx]; }
    void SwapQ();
    int m_recvIdx;   // +0
    int m_parseIdx;  // +4
    T m_queues[N];   // +8
};

// ---- 队列 typedef（原 ManagerApp.h）----
typedef std::queue<CUdpRecvBuffer*, std::deque<CUdpRecvBuffer*, std::allocator<CUdpRecvBuffer*> > > UdpRecvQueue;
typedef std::queue<CTcpRecvBuffer*, std::deque<CTcpRecvBuffer*, std::allocator<CTcpRecvBuffer*> > > TcpRecvQueue;
typedef std::queue<CTcpSendBuffer*, std::deque<CTcpSendBuffer*, std::allocator<CTcpSendBuffer*> > > CTcpSendQueue;

#endif  // DBMW_DNFPACKETBUFFER_H_
