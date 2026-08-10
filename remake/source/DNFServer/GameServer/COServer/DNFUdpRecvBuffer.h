#ifndef DNF_UDP_RECV_BUFFER_H_
#define DNF_UDP_RECV_BUFFER_H_

#include <vector>

class CUdpRecvBuffer;

// ---- 池（MemPool<CUdpRecvBuffer>：classSize@0 / count@4 / chunks vector@8）----
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
    int m_classSize;             // +0
    unsigned int m_count;        // +4
    std::vector<void*> m_chunks; // +8
};

// CUdpRecvBuffer 池静态成员（容量 10000）
extern MemPool<CUdpRecvBuffer> m_RecvBufferMemPool_;

// CUdpRecvBuffer：UDP 收包缓冲（MemPool 元素，0x204 原始字节，无自定义构造/析构）
class CUdpRecvBuffer
{
public:
    static void* operator new(unsigned int size);
    static void operator delete(void* p);
    static void operator delete(void* p, unsigned int size);
    char m_data[0x204];
};

#endif // DNF_UDP_RECV_BUFFER_H_
