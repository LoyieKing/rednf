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
    void* result;
    if (m_classSize == 0x1804)
    {
        if (headOfFreeList_ == 0)
        {
            void* block = ::operator new(m_classSize * m_count);
            for (unsigned int i = 0; i < m_count - 1U; i++)
            {
                *(void**)((int)block + i * 0x1804 + 0x1800) =
                    (void*)((i + 1) * 0x1804 + (int)block);
            }
            *(void**)((int)block + (m_count - 1) * 0x1804 + 0x1800) = 0;
            headOfFreeList_ = (void*)((int)block + 0x1804);
            result = block;
            m_chunks.push_back(std::move(block));
            DNF_LOG_SCOPE_LINE(0x7d, "./log/Mempool", "class size(%d) cnt(%d)", m_classSize, m_count * (int)m_chunks.size());
        }
        else
        {
            result = headOfFreeList_;
            headOfFreeList_ = *(void**)((int)headOfFreeList_ + 0x200);
        }
    }
    else
    {
        result = ::operator new(0x1804);
    }
    return result;
}
template<class T>
void MemPool<T>::free(void* p)
{
    if (p != 0)
    {
        *(void**)((int)p + 0x1800) = headOfFreeList_;
        headOfFreeList_ = p;
    }
}
template<class T>
void MemPool<T>::free(void* p, unsigned int size)
{
    if (p != 0)
    {
        if (m_classSize == (int)size)
        {
            *(void**)((int)p + 0x1800) = headOfFreeList_;
            headOfFreeList_ = p;
        }
        else
        {
            ::operator delete(p);
        }
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
