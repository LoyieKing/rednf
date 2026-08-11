// df_coserver_r — MemPool<CUdpRecvBuffer>/CUdpRecvBuffer（ORIG DNFUdpRecvBuffer.cpp）
#include <stdio.h>
#include <stdlib.h>

#include "DNFUdpRecvBuffer.h"
#include "DNFFileLog.h"

// ---- MemPool<CUdpRecvBuffer> ----
template<class T>
void* MemPool<T>::headOfFreeList_ = 0;

MemPool<CUdpRecvBuffer> m_RecvBufferMemPool_(10000);

template<class T>
MemPool<T>::MemPool(unsigned int count)
    : m_classSize(0x204), m_count(count)
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
    // ORIG 实测（0x805858e）：result@-0x14 / block@-0x10 两个独立槽位，
    // 第一条分支直接 return ::operator new（不经 result 槽），
    // 链指针用成员位移形态（mov %eax,0x200(%edx)）。
    void* result;
    CUdpRecvBuffer* block;
    if (m_classSize != 0x204)
    {
        return ::operator new(0x204);
    }
    result = headOfFreeList_;
    if (result != 0)
    {
        headOfFreeList_ = ((CUdpRecvBuffer*)result)->m_next;
    }
    else
    {
        block = (CUdpRecvBuffer*)::operator new(m_count * m_classSize);
        for (unsigned int i = 0; i < m_count - 1U; i++)
        {
            block[i].m_next = (void*)&block[i + 1];
        }
        block[m_count - 1].m_next = 0;
        result = block;
        headOfFreeList_ = (void*)((char*)block + 0x204);
        m_chunks.push_back(static_cast<void*&&>((void*)block));
        DNF_LOG_SCOPE_LINE(0x7d, "./log/Mempool", "class size(%d) cnt(%d)", m_classSize, m_count * (int)m_chunks.size());
    }
    return result;
}

template<class T>
void MemPool<T>::free(void* p)
{
    if (p != 0)
    {
        CUdpRecvBuffer* q = (CUdpRecvBuffer*)p;
        q->m_next = headOfFreeList_;
        headOfFreeList_ = q;
        // ORIG 实测（0x8058732 尾）：if 块末有显式 jmp+对齐 nop 的共享出口。
        return;
    }
    return;
}

template<class T>
void MemPool<T>::free(void* p, unsigned int size)
{
    if (p != 0)
    {
        if (m_classSize != size)
        {
            ::operator delete(p);
        }
        else
        {
            CUdpRecvBuffer* q = (CUdpRecvBuffer*)p;
            q->m_next = headOfFreeList_;
            headOfFreeList_ = q;
            return;
        }
    }
    return;
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
