// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 StreamPool/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
#include "CStreamGuard.h"   // StreamPool::Free(CStreamGuard&) 取流用
// df_game_r 还原 —— StreamPool / SmallStreamPool / BigStreamPool（S1 组）
#include "CEnvironment.h"
#include "StreamPool.h"

#include "StreamPool/StreamPool.inc"
{
    int n = G_CEnvironment()->m_serverEnvir.m_defaultPoolSize;
    m_pool = new DynamicPool<Stream>(n);
    m_pool->SetPoolSize((unsigned short)n, 1000);
    m_pool->m_pCheckCallback = 0;
}

#include "StreamPool/dtor_StreamPool.inc"
{
    if (m_pool)
    {
        delete m_pool;
        m_pool = 0;
    }
}

#include "StreamPool/Acquire.inc"
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

#include "StreamPool/Free.inc"
{
    if (!s)
    {
        return false;
    }
    Guard<Mutex> guard(&m_mutex);
    s->m_inUse = 0;
    if (!m_pool->IsValidPtr(s))
    {
        LogManager::logFormat(1, "App.cpp", __PRETTY_FUNCTION__, 0x239,
                              "[StreamPool::Free] invalid ptr : %p", s);
        return false;
    }
    s->reset();
    m_pool->Free(s);
    return true;
}

// ORIG _ZN10StreamPool4FreeER12CStreamGuard W 0x08101080（弱内联，无独立报告）：
// objdump 0x8101080——取 guard 持有的 Stream* 后复用 Free(Stream*)。
#include "StreamPool/Free_1.inc"
{
    return Free(guard.Get());
}

#include "StreamPool/Size.inc"
{
    return m_pool->Size();
}

#include "StreamPool/Remain.inc"
{
    Guard<Mutex> guard(&m_mutex);
    return m_pool->Remain();
}

#include "StreamPool/SmallStreamPool.inc"
{
    int n = G_CEnvironment()->m_serverEnvir.m_smallPoolSize;
    m_pool = new DynamicPool<SmallStream>(n);
    m_pool->SetPoolSize((unsigned short)n, 1000);
    m_pool->m_pCheckCallback = 0;
}

#include "StreamPool/dtor_SmallStreamPool.inc"
{
    if (m_pool)
    {
        delete m_pool;
        m_pool = 0;
    }
}

#include "StreamPool/Acquire_1.inc"
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

#include "StreamPool/Free_2.inc"
{
    SmallStream* p = (SmallStream*)s;
    if (!p)
    {
        return false;
    }
    Guard<Mutex> guard(&m_mutex);
    if (!m_pool->IsValidPtr(p))
    {
        LogManager::logFormat(1, "App.cpp", __PRETTY_FUNCTION__, 0x2ae,
                              "[BigStreamPool::Free] invalid ptr : %p", p);
        return false;
    }
    p->reset();
    m_pool->Free(p);
    return true;
}

#include "StreamPool/Size_1.inc"
{
    return m_pool->Size();
}

#include "StreamPool/Remain_1.inc"
{
    Guard<Mutex> guard(&m_mutex);
    return m_pool->Remain();
}

#include "StreamPool/BigStreamPool.inc"
{
    int n = G_CEnvironment()->m_serverEnvir.m_bigPoolSize;
    m_pool = new DynamicPool<BigStream>(n);
    m_pool->SetPoolSize((unsigned short)n, 100);
    m_pool->m_pCheckCallback = 0;
}

#include "StreamPool/dtor_BigStreamPool.inc"
{
    if (m_pool)
    {
        delete m_pool;
        m_pool = 0;
    }
}

#include "StreamPool/Acquire_2.inc"
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

#include "StreamPool/Free_3.inc"
{
    BigStream* p = (BigStream*)s;
    if (!p)
    {
        return false;
    }
    Guard<Mutex> guard(&m_mutex);
    if (!m_pool->IsValidPtr(p))
    {
        LogManager::logFormat(1, "App.cpp", __PRETTY_FUNCTION__, 0x2fc,
                              "[BigStreamPool::Free] invalid ptr : %p", p);
        return false;
    }
    p->reset();
    m_pool->Free(p);
    return true;
}

#include "StreamPool/Size_2.inc"
{
    return m_pool->Size();
}

#include "StreamPool/Remain_2.inc"
{
    Guard<Mutex> guard(&m_mutex);
    return m_pool->Remain();
}
