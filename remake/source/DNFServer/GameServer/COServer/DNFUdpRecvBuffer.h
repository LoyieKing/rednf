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
    // ORIG 实测：池化元素 0x204 字节 = 数据区 0x200 + 空闲链指针@0x200。
    // 用成员形态访问（mov 0x200(%eax),%eax / mov %edx,0x200(%eax)），
    // 指针算术形态会退化成 lea+间接存储（与 ORIG 不符）。
    char m_data[0x200];
    void* m_next;  // +0x200（MemPool 空闲链）
};

#endif // DNF_UDP_RECV_BUFFER_H_
