#ifndef DNF_GAME_GAMETYPES_H_
#define DNF_GAME_GAMETYPES_H_

// df_game_r Game/ 全局基础类型唯一权威定义（2026-08-16 集成）。
// 替代各 subagent 临时头（GameBase.h / StreamDeps.h）中的重复声明。
// Mutex 布局 0x18（pthread_mutex_t）；DynamicPool 布局 0x5c（ORIG 池对象尺寸）。
// DynamicPool<T> 为 ORIG 真实实现（objdump 0x82ac4cc 起）：queue/list/map 管理
// CNode 块；MoreAlloc 用 new CNode[n]（块头 cookie=n），dtor 用 delete[] 释放。

#include <pthread.h>

#include <cstddef>
#include <list>
#include <map>
#include <new>
#include <queue>

// Uncopyable：空基类（ORIG 0x8ad38a2，C1/D1 为空但非内联）
class Uncopyable
{
public:
    Uncopyable();
    ~Uncopyable();
};

class Mutex
{
public:
    Mutex()  { pthread_mutex_init(&m_mutex, 0); }
    ~Mutex() { pthread_mutex_destroy(&m_mutex); }
    void lock()   { pthread_mutex_lock(&m_mutex); }
    void unlock() { pthread_mutex_unlock(&m_mutex); }
private:
    pthread_mutex_t m_mutex;   // +0x00，0x18 字节
};

template <class T>
class Guard
{
public:
    Guard(T* p) { p->lock(); m_lock = p; }
    ~Guard()    { m_lock->unlock(); }
private:
    T* m_lock;
};

// ---- 依赖声明（真实定义属其它 TU）----
class cMyTrace
{
public:
    cMyTrace(const char* name, int line, int flag);
    cMyTrace(const char* name, int line, int flag, bool bFieldC, bool bFieldD);  // ORIG 0x854f746
    void operator()(const char* fmt, ...);

private:
    const char* m_name;   // +0x00
    int m_line;           // +0x04
    int m_flag;           // +0x08
    char m_field_c;       // +0x0c
    char m_field_d;       // +0x0d（对象 0xe 字节）
};

#include "LogManager.h"

void PrintBackTrace();

template <class T>
class DynamicPool
{
public:
    struct CNode : public T
    {
        CNode() : m_type(0x8f21), m_inUse(0) {}

        int m_type;    // +0x28（池类型魔数 0x8f21）
        char m_inUse;  // +0x2c（池占用计数）
    };

    DynamicPool(int poolSize);
    ~DynamicPool();
    T* Acquire();
    bool IsValidPtr(T* p);
    bool Free(T* p);
    int Size();
    int Remain();
    void SetPoolSize(unsigned short baseSize, unsigned short maxSize);
    bool MoreAlloc(int n);

    void (*m_pCheckCallback)();  // +0x00
private:
    int m_errorCode;                 // +0x04
    std::queue<CNode*> m_queue;      // +0x08（0x28 字节）
    std::list<CNode*> m_list;        // +0x30（0x8 字节，所有 CNode 块）
    int m_size;                      // +0x38（池对象总数）
    int m_used;                      // +0x3c（未使用计数）
    unsigned short m_baseSize;       // +0x40
    unsigned short m_maxSize;        // +0x42
    std::map<void*, int> m_map;      // +0x44
};

// ---------------------------------------------------------------------------
// DynamicPool<T> 真实实现（ORIG 语义；成员初始化顺序与 ORIG 逐条一致）
// ---------------------------------------------------------------------------

template <class T>
DynamicPool<T>::DynamicPool(int poolSize)
    : m_queue(), m_list(), m_size(0), m_map()
{
    m_used = 0;
    m_list.clear();
    m_baseSize = (unsigned short)poolSize;
    m_maxSize = m_baseSize;
    MoreAlloc(poolSize);
    m_errorCode = 0;
    m_pCheckCallback = 0;
}

template <class T>
DynamicPool<T>::~DynamicPool()
{
    typename std::list<CNode*>::iterator it = m_list.begin();
    typename std::list<CNode*>::iterator end = m_list.end();
    while (it != end)
    {
        CNode* node = *it;
        delete[] node;
        ++it;
    }
    m_list.clear();
    m_pCheckCallback = 0;
}

template <class T>
T* DynamicPool<T>::Acquire()
{
    if (m_queue.empty())
    {
        MoreAlloc(m_maxSize);
        if (m_queue.size() > m_maxSize)
        {
            cMyTrace(__FUNCTION__, 0x19c, 0)
                ("DYNAMIC MEMORY_POOL More Alloc Size(%d)", m_queue.size());
        }
        if (m_pCheckCallback)
        {
            m_pCheckCallback();
        }
    }
    if (m_queue.empty())
    {
        LogManager::logFormat(1, "Container.h", "Acquire", 0x1a6,
                              "[Pool Err] Insufficient memory!");
        return 0;
    }
    CNode* node = m_queue.front();
    T* ret = node;
    if (node->m_type != 0x8f21)
    {
        cMyTrace(__FUNCTION__, 0x1b3, 0)
            ("DYNAMIC MEMORY_POOL Acquire error dummy(%x), node(%x)", node->m_type, node);
        m_errorCode = 3;
    }
    if (node->m_inUse != 0)
    {
        cMyTrace(__FUNCTION__, 0x1b9, 0)
            ("DYNAMIC MEMORY_POOL Acquire error reference count(%d), node(%x)",
             node->m_inUse, node);
        m_errorCode = 4;
    }
    ++node->m_inUse;
    if (!ret)
    {
        cMyTrace(__FUNCTION__, 0x1c4, 5)
            ("DYNAMIC MEMORY_POOL Acquire error [%s][%d]", __FUNCTION__, 0x1c4);
    }
    m_queue.pop();
    return ret;
}

template <class T>
bool DynamicPool<T>::IsValidPtr(T* p)
{
    void* key = p;
    std::map<void*, int>::iterator it = m_map.find(key);
    return it == m_map.end() ? false : true;
}

template <class T>
bool DynamicPool<T>::Free(T* p)
{
    if (!p)
    {
        return false;
    }
    CNode* node = (CNode*)p;
    m_queue.push(node);
    if (node->m_type != 0x8f21)
    {
        cMyTrace(__FUNCTION__, 0x269, 0)
            ("DYNAMIC MEMORY_POOL Free error dummy(%x), node(%x)", node->m_type, node);
        m_errorCode = 5;
    }
    if (node->m_inUse != 1)
    {
        cMyTrace(__FUNCTION__, 0x26f, 0)
            ("DYNAMIC MEMORY_POOL Free error reference count(%d), node(%x)",
             node->m_inUse, node);
        m_errorCode = 6;
        PrintBackTrace();
    }
    --node->m_inUse;
    return true;
}

template <class T>
int DynamicPool<T>::Size()
{
    return m_size;
}

template <class T>
int DynamicPool<T>::Remain()
{
    return (int)m_queue.size();
}

template <class T>
void DynamicPool<T>::SetPoolSize(unsigned short baseSize, unsigned short maxSize)
{
    m_baseSize = baseSize;
    m_maxSize = maxSize;
}

template <class T>
bool DynamicPool<T>::MoreAlloc(int n)
{
    CNode* nodes = new CNode[n];
    if (!nodes)
    {
        cMyTrace(__FUNCTION__, 0x2a6, 5)
            ("DYNAMIC MEMORY_POOL Acquire error [%s][%d]", __FUNCTION__, 0x2a6);
        return false;
    }
    m_list.push_back(nodes);
    for (int i = 0; i < n; ++i)
    {
        m_queue.push(nodes + i);
        m_map[nodes + i] = 1;
    }
    m_size += n;
    return true;
}

#endif  // DNF_GAME_GAMETYPES_H_
