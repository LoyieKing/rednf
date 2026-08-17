// df_game_r 还原 —— StreamPool / SmallStreamPool / BigStreamPool（S1 组）
#include "CEnvironment.h"
#include "StreamPool.h"

StreamPool::StreamPool()
{
    int n = G_CEnvironment()->m_serverEnvir.m_defaultPoolSize;
    m_pool = new DynamicPool<Stream>(n);
    m_pool->SetPoolSize((unsigned short)n, 1000);
    m_pool->m_pCheckCallback = 0;
}

StreamPool::~StreamPool()
{
    if (m_pool)
    {
        delete m_pool;
        m_pool = 0;
    }
}

Stream* StreamPool::Acquire(const char* file, int line)
{
    Guard<Mutex> guard(&m_mutex);
    Stream* s = m_pool->Acquire();
    if (!s)
    {
        return 0;
    }
    s->m_file = file;
    s->m_line = line;
    s->m_poolType = 0;
    s->set_big_stream(false);
    s->m_inUse = 1;
    return s;
}

bool StreamPool::Free(Stream* s)
{
    if (!s)
    {
        return false;
    }
    Guard<Mutex> guard(&m_mutex);
    s->m_inUse = 0;
    if (!m_pool->IsValidPtr(s))
    {
        LogManager::logFormat(1, "App.cpp", "bool StreamPool::Free(Stream*)", 0x239,
                              "[StreamPool::Free] invalid ptr : %p", s);
        return false;
    }
    s->reset();
    m_pool->Free(s);
    return true;
}

int StreamPool::Size()
{
    return m_pool->Size();
}

int StreamPool::Remain()
{
    Guard<Mutex> guard(&m_mutex);
    return m_pool->Remain();
}

SmallStreamPool::SmallStreamPool()
{
    int n = G_CEnvironment()->m_serverEnvir.m_smallPoolSize;
    m_pool = new DynamicPool<SmallStream>(n);
    m_pool->SetPoolSize((unsigned short)n, 1000);
    m_pool->m_pCheckCallback = 0;
}

SmallStreamPool::~SmallStreamPool()
{
    if (m_pool)
    {
        delete m_pool;
        m_pool = 0;
    }
}

Stream* SmallStreamPool::Acquire(const char* file, int line)
{
    Guard<Mutex> guard(&m_mutex);
    Stream* s = m_pool->Acquire();
    if (!s)
    {
        return 0;
    }
    s->m_file = file;
    s->m_line = line;
    s->m_poolType = 1;
    s->m_inUse = 1;
    return s;
}

bool SmallStreamPool::Free(Stream* s)
{
    SmallStream* p = (SmallStream*)s;
    if (!p)
    {
        return false;
    }
    Guard<Mutex> guard(&m_mutex);
    if (!m_pool->IsValidPtr(p))
    {
        LogManager::logFormat(1, "App.cpp", "bool SmallStreamPool::Free(Stream*)", 0x2ae,
                              "[BigStreamPool::Free] invalid ptr : %p", p);
        return false;
    }
    p->reset();
    m_pool->Free(p);
    return true;
}

int SmallStreamPool::Size()
{
    return m_pool->Size();
}

int SmallStreamPool::Remain()
{
    Guard<Mutex> guard(&m_mutex);
    return m_pool->Remain();
}

BigStreamPool::BigStreamPool()
{
    int n = G_CEnvironment()->m_serverEnvir.m_bigPoolSize;
    m_pool = new DynamicPool<BigStream>(n);
    m_pool->SetPoolSize((unsigned short)n, 100);
    m_pool->m_pCheckCallback = 0;
}

BigStreamPool::~BigStreamPool()
{
    if (m_pool)
    {
        delete m_pool;
        m_pool = 0;
    }
}

Stream* BigStreamPool::Acquire(const char* file, int line)
{
    Guard<Mutex> guard(&m_mutex);
    Stream* s = m_pool->Acquire();
    if (!s)
    {
        return 0;
    }
    s->m_file = file;
    s->m_line = line;
    s->m_poolType = 2;
    s->set_big_stream(true);
    s->m_inUse = 1;
    return s;
}

bool BigStreamPool::Free(Stream* s)
{
    BigStream* p = (BigStream*)s;
    if (!p)
    {
        return false;
    }
    Guard<Mutex> guard(&m_mutex);
    if (!m_pool->IsValidPtr(p))
    {
        LogManager::logFormat(1, "App.cpp", "bool BigStreamPool::Free(Stream*)", 0x2fc,
                              "[BigStreamPool::Free] invalid ptr : %p", p);
        return false;
    }
    p->reset();
    m_pool->Free(p);
    return true;
}

int BigStreamPool::Size()
{
    return m_pool->Size();
}

int BigStreamPool::Remain()
{
    Guard<Mutex> guard(&m_mutex);
    return m_pool->Remain();
}
