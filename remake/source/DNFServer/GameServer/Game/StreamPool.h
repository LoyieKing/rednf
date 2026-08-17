#ifndef DNF_GAME_STREAMPOOL_H_
#define DNF_GAME_STREAMPOOL_H_

// ============================================================================
// df_game_r 还原 —— StreamPool / SmallStreamPool / BigStreamPool（S1 组）
// 参照 docs/class_func_reports/{StreamPool,SmallStreamPool,BigStreamPool}.md
// 布局：+0x00 Mutex | +0x18 DynamicPool<T>*（sizeof = 0x1c）
// ============================================================================

#include "Stream.h"
#include "StreamDeps.h"
#include "CEnvironment.h"

class StreamPool
{
public:
    StreamPool();                    // 0x0828f974
    ~StreamPool();                   // 0x0828fa20
    Stream* Acquire(const char* file, int line);   // 0x0828fa86
    bool Free(Stream* s);            // 0x0828fb28
    int Size();                      // 0x0828fc06
    int Remain();                    // 0x0828fc1c

private:
    Mutex m_mutex;                   // +0x00
    DynamicPool<Stream>* m_pool;     // +0x18
};

class SmallStreamPool
{
public:
    SmallStreamPool();               // 0x0828fe62
    ~SmallStreamPool();              // 0x0828ff0e
    Stream* Acquire(const char* file, int line);   // 0x0828ff74
    bool Free(Stream* s);            // 0x08290004
    int Size();                      // 0x082900e0
    int Remain();                    // 0x082900f6

private:
    Mutex m_mutex;                   // +0x00
    DynamicPool<SmallStream>* m_pool;  // +0x18
};

class BigStreamPool
{
public:
    BigStreamPool();                 // 0x08290150
    ~BigStreamPool();                // 0x082901fc
    Stream* Acquire(const char* file, int line);   // 0x08290262
    bool Free(Stream* s);            // 0x08290304
    int Size();                      // 0x082903e0
    int Remain();                    // 0x082903f6

private:
    Mutex m_mutex;                   // +0x00
    DynamicPool<BigStream>* m_pool;  // +0x18
};

#endif  // DNF_GAME_STREAMPOOL_H_
