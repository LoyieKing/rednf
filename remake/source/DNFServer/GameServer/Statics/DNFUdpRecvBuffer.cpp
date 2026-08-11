// df_statics_r — DNFUdpRecvBuffer（ORIG DNFUdpRecvBuffer.o 拆分）
#include <stdio.h>
#include <utility>

#include "DNFUdpRecvBuffer.h"
#include "DNFFileLog.h"

template<class T>
void* MemPool<T>::headOfFreeList_ = 0;
MemPool<CUdpRecvBuffer> m_RecvBufferMemPool_(10000);
template<class T>
MemPool<T>::MemPool(unsigned int count)
    : m_classSize(0x1804), m_count(count)
{
}
template<class T>
MemPool<T>::~MemPool()
{
    if (!m_chunks.empty())
    {
        for (std::vector<void*>::iterator it = m_chunks.begin(); it != m_chunks.end(); ++it)
        {
            ::operator delete(*it);
        }
        m_chunks.clear();
    }
}
template<class T>
void* MemPool<T>::alloc()
{
    struct __attribute__((packed)) MemLink
    {
        char m_pad[0x1800];
        void* m_next;
    };
    if (m_classSize != 0x1804)
        return ::operator new(0x1804);
    void* result = headOfFreeList_;
    if (result != 0)
    {
        headOfFreeList_ = ((MemLink*)result)->m_next;
    }
    else
    {
        MemLink* block = (MemLink*)::operator new(m_count * m_classSize);
        for (unsigned int i = 0; i < m_count - 1U; i++)
        {
            block[i].m_next = &block[i + 1];
        }
        block[m_count - 1].m_next = 0;
        result = block;
        headOfFreeList_ = &block[1];
        m_chunks.push_back((void*)block);
        DNF_LOG_SCOPE_LINE(0x7d, "./log/Mempool", "class size(%d) cnt(%d)", m_classSize, m_count * (int)m_chunks.size());
    }
    return result;
}
template<class T>
void MemPool<T>::free(void* p)
{
    if (p == 0)
        return;
    {
        struct __attribute__((packed)) MemLink
        {
            char m_pad[0x1800];
            void* m_next;
        };
        MemLink* q = (MemLink*)p;
        q->m_next = headOfFreeList_;
        headOfFreeList_ = q;
    }
}
template<class T>
void MemPool<T>::free(void* p, unsigned int size)
{
    if (p == 0)
        return;
    if (m_classSize != size)
    {
        ::operator delete(p);
    }
    else
    {
        struct __attribute__((packed)) MemLink
        {
            char m_pad[0x1800];
            void* m_next;
        };
        MemLink* q = (MemLink*)p;
        q->m_next = headOfFreeList_;
        headOfFreeList_ = q;
    }
}
void* CUdpRecvBuffer::operator new(unsigned int size)
{
    return m_RecvBufferMemPool_.alloc();
}
void CUdpRecvBuffer::operator delete(void* p)
{
    m_RecvBufferMemPool_.free(p);
}
void CUdpRecvBuffer::operator delete(void* p, unsigned int size)
{
    m_RecvBufferMemPool_.free(p, size);
}
