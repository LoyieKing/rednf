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
#include <string>
#include <vector>

// [P7HeaderAxis 2026-09-11] ORIG 直证（df_game_r）：
//   prett 串 @0x08c1b180 size 0x3e 实读
//     `char* GetConfigFileString(tag_name_value_map_t&, const char*)`
//   同函数 mangled `_Z19GetConfigFileStringRSt3mapISsSsSt4lessISsESaISt4pairIKSsSsEEEPKc`
//   第二参展开为 std::map<std::string, std::string>& ⇒ 本 typedef 底层类型唯一确定
//   （typedef 名不可伪装：prett 按源码拼写打印，mangled 按底层类型展开，两者同时吻合）。
typedef std::map<std::string, std::string> tag_name_value_map_t;

// [P7HeaderAxis 2026-09-11] ORIG 直证（df_game_r prett 串，共 15+ 处）：
//   `virtual bool TimerCheckConn::dispatch_sig(int, int, memberIdentificationNumber_t)`
//   @ORIG prett size 0x52；同函数 mangled `_ZN14TimerCheckConn12dispatch_sigEiij`
//   第 3 参展开为 `unsigned int`（j）⇒ 本 typedef 底层类型唯一确定。
//   全库既有 12+ 处 TU 局部同型定义（`typedef unsigned int memberIdentificationNumber_t;`）
//   与本定义类型相同，重复 typedef 合法；此处集中定义供各 TU 签名拼写复用。
typedef unsigned int memberIdentificationNumber_t;

// [P7HeaderAxis 2026-09-11] ORIG 全局作用域 typedef 补全（prett 串直证；全库此前缺失
// ⇒ 各 TU 只能把手写串或展开类型写进 __PRETTY_FUNCTION__，与 ORIG 不符）。
// 底层类型均由同函数 mangled 唯一确定（mangled 不含 typedef 名，只含底层类型）。
//   IntPairVector   : `void makeRewardItemInfo(const INT_VECTOR&, IntPairVector&)`
//                     @ORIG prett ×6（Quest/CUser::_ProcCompoundCore/expert_extraction×3/
//                     EventClassify::CExchangeRandomItemReward/WongWork::CItemUpgrade），
//                     mangled 展开 `St6vectorISt4pairIiiESaIS2_EE`
//   INT_VECTOR      : `int CBattle_Field::set_map_id(const GridScript*, int&, INT_VECTOR&)`，`St6vectorIiSaIiEE`
//   StrVector       : `int Dispatcher_New_Gmdebug_Command::oldGmRoutine(CUser*, PacketBuf&, StrVector&)`，`St6vectorISsSaISsEE`
//   itemIndexOnlyServer_t : `void Arad_IPGProcess::CashShopModeVendingItem(CUser*, itemIndexOnlyServer_t, ...)`，`m`=unsigned long
//                     （InterDispatcher.h:2481 有同型定义；本处集中后对不含该头的 TU 亦可见，重复 typedef 合法）
//   RequestItemCountType : `bool CUser::_ProcCompoundCore(..., RequestItemCountType, ...)`，`t`=unsigned short
//   NCHAR           : `int CNPCScriptList::initScript(NCHAR*, NCHAR*, NCHAR*)`，`Pc`=char*
typedef std::vector<std::pair<int, int> > IntPairVector;
typedef std::vector<int> INT_VECTOR;
typedef std::vector<std::string> StrVector;
typedef unsigned long itemIndexOnlyServer_t;
typedef unsigned short RequestItemCountType;
typedef char NCHAR;
// [P7HeaderAxis 2026-09-11] 另 3 个 ORIG 全局 typedef（P139 prett 直证；全库此前无定义，
// 各 TU 只能用手写串）：
//   itemindex_t  : `bool ARAD::FnRewardLethe::operator()(int, itemindex_t, short int, arad_uint32)`
//                  （ARAD::FnRewardLethe / FnRewardInterface 两处；mangled `…clEimsjE` 中 m=unsigned long）
//   itemIndex_t  : `random_option::CRandomOptionItemHandle::change_option(itemIndex_t, ENUM_RARITY, …)`
//                  等 5 处（UserMiscLink TU 实证，P8S1 探针确认宏按定义处拼写展开）
//   arad_uint32  : 同上 ARAD prett，mangled `j`=unsigned int
typedef unsigned long itemindex_t;
typedef unsigned long itemIndex_t;
typedef unsigned int arad_uint32;

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

class rwLock
{
public:
    rwLock() { pthread_rwlock_init(&m_lock, 0); }
    ~rwLock() { pthread_rwlock_destroy(&m_lock); }
    void rdlock() { pthread_rwlock_rdlock(&m_lock); }
    void wrlock() { pthread_rwlock_wrlock(&m_lock); }
    void unlock() { pthread_rwlock_unlock(&m_lock); }
private:
    pthread_rwlock_t m_lock;   // +0x00，0x20 字节
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

template <class T>
class readGuard
{
public:
    readGuard(T* p)
    {
        p->rdlock();
        m_lock = p;
    }
    ~readGuard() { m_lock->unlock(); }
private:
    T* m_lock;
};

template <class T>
class writeGuard
{
public:
    writeGuard(T* p)
    {
        p->wrlock();
        m_lock = p;
    }
    ~writeGuard() { m_lock->unlock(); }
private:
    T* m_lock;
};

// ---- 依赖声明（真实定义属其它 TU）----
class cMyTrace
{
public:
    // [Main·2026-09-11 全局 A/B 实验] `throw()`（C++98 空异常规格）告知编译器该 ctor 不抛
    // ——4.4 在调用点**看不到定义**时布 EH landing pad（含 `__Unwind_Resume`），而 ORIG 侧
    // 无该 pad（K2 在 CPartyDepsD2::check_allmember_loading 实证：我方多 13 条 EH cleanup）。
    // 影响面 = 全库所有 `cMyTrace` 调用点（热点头）⇒ 必须以**全局核账**定稿：
    // 若 identical 不升或 diff/near 上升即整批回滚。
    cMyTrace(const char* name, int line, int flag) throw();
    cMyTrace(const char* name, int line, int flag, bool bFieldC, bool bFieldD) throw();  // ORIG 0x854f746
    void operator()(const char* fmt, ...);

private:
    const char* m_name;   // +0x00
    int m_line;           // +0x04
    int m_flag;           // +0x08
    // [b28c-35] ORIG cMyTrace::C1 5 参 0x0854f746 尾 store 为 this 先
    // （`mov 0x8(%ebp),%eax; movzbl -0x8(%ebp),%edx; mov %dl,0xc(%eax)`）⇒ 字段与
    // bool 形参同型（P211 LHS-first）；char 字段产值先形。对象仍 0xe 字节。
    bool m_field_c;       // +0x0c
    bool m_field_d;       // +0x0d
};

#include "LogManager.h"

void PrintBackTrace();

template <class T>
class DynamicPool
{
public:
    struct CNode : public T
    {
        // 声明后外联定义（见下方），供多态 T（CEgg/CCreature）以显式特化注入
        // CNode vptr 重写（ORIG 在基类 T() 后先写 CNode 虚表再写 magic/bUse）。
        CNode();
        ~CNode();

        int m_type;    // +0x28（池类型魔数 0x8f21）
        char m_inUse;  // +0x2c（池占用计数）
    };

    DynamicPool(int poolSize);
    // ORIG W 0x082fa3fc（C2 同址）：(baseSize, maxSize) 双参。
    DynamicPool(int baseSize, int maxSize);
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

// ORIG W 0x082fa3fc：m_baseSize=arg1、m_maxSize=arg2、MoreAlloc(arg1)；
// 其余与 1 参版一致（deque/queue/list/map 成员构造 + m_size/m_used 清零）。
template <class T>
DynamicPool<T>::DynamicPool(int baseSize, int maxSize)
    : m_queue(), m_list(), m_size(0), m_map()
{
    m_used = 0;
    m_list.clear();
    m_baseSize = (unsigned short)baseSize;
    m_maxSize = (unsigned short)maxSize;
    MoreAlloc(baseSize);
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

// ---- DynamicPool<T>::CNode 外联构造/析构（多态 T 的 vptr 重写由显式特化注入）----
template <class T>
DynamicPool<T>::CNode::CNode()
{
    m_type = 0x8f21;
    m_inUse = 0;
}

template <class T>
DynamicPool<T>::CNode::~CNode()
{
}

template <class T>
bool DynamicPool<T>::MoreAlloc(int n)
{
    // b28 BM3：cookie 有无由 CNode 析构 trivial 性决定——SIPGData 实例经
    // TaiwanCash.h 整类特化去用户 dtor 后，4.4 `new CNode[n]` 发
    // `imul $0x2e4,%esi,%eax; call _Znaj` 直返无 cookie，与 ORIG 0x081019c2
    // 同形（显式 ::operator new[] 形会丢 ctor 循环，探针 /tmp/bm3/p20 实证）。
    // 其余实例化（CEgg 等）orig 形态不受本 TU 影响（特化仅 SIPGData）。
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
