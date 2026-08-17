// df_game_r 还原 —— CStreamGuard 全量（S1 组）
#include "CStreamGuard.h"

// GlobalData 静态池指针（属 GlobalData 组还原范围，此处仅声明引用）
class GlobalData
{
public:
    static StreamPool* s_stream_pool;        // 0x0940bd6c
    static BigStreamPool* s_big_stream_pool; // 0x0940bda4
    static SmallStreamPool* s_small_stream_pool;  // 0x0940bda8
};

CStreamGuard::CStreamGuard(bool bOwn)
{
    m_bOwn = bOwn;
}

CStreamGuard::CStreamGuard(Stream* stream, bool bOwn)
{
    m_stream = stream;
    m_bOwn = bOwn;
}

CStreamGuard::~CStreamGuard()
{
    if (m_stream && m_bOwn && m_stream->m_inUse)
    {
        // ORIG cmp 级联顺序：1→Small、2→Big、default→Stream
        if (m_stream->m_poolType == 1)
        {
            GlobalData::s_small_stream_pool->Free(m_stream);
        }
        else if (m_stream->m_poolType == 2)
        {
            GlobalData::s_big_stream_pool->Free(m_stream);
        }
        else
        {
            GlobalData::s_stream_pool->Free(m_stream);
        }
    }
}

void CStreamGuard::Set(Stream* stream)
{
    m_stream = stream;
}

Stream* CStreamGuard::Get()
{
    return m_stream;
}

CStreamGuard* CStreamGuard::operator->()
{
    return this;
}

CStreamGuard* CStreamGuard::operator*()
{
    return this;
}

Stream& CStreamGuard::operator<<(bool b)
{
    return *m_stream << b;
}

Stream& CStreamGuard::operator<<(char c)
{
    return *m_stream << c;
}

Stream& CStreamGuard::operator<<(short s)
{
    return *m_stream << s;
}

Stream& CStreamGuard::operator<<(int v)
{
    return *m_stream << v;
}

Stream& CStreamGuard::operator<<(long v)
{
    return *m_stream << v;
}

Stream& CStreamGuard::operator<<(unsigned char c)
{
    return *m_stream << c;
}

Stream& CStreamGuard::operator<<(unsigned short s)
{
    return *m_stream << s;
}

Stream& CStreamGuard::operator<<(unsigned int v)
{
    return *m_stream << v;
}

Stream& CStreamGuard::operator<<(unsigned long v)
{
    return *m_stream << v;
}

Stream& CStreamGuard::operator<<(char* s)
{
    return *m_stream << s;
}

int CStreamGuard::put_binary(void* src, int len)
{
    return m_stream->put_binary(src, len);
}
