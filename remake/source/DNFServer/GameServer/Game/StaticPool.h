#ifndef GAME_STATICPOOL_H_
#define GAME_STATICPOOL_H_

// ============================================================================
// StaticPool<T,N> —— ORIG 真实实现（objdump 0x82ae390 起逐函数核对）
// （2026-08-23 自 CGameManager.h 提取为独立叶子头，供 CMailBoxHelper.h 等
//   轻量消费者使用；实现见 CGameManager.cpp。）
// ============================================================================

#include <queue>
#include <cstdio>   // b28d-lane12：_IO_FILE 全特化需完整类型（struct _IO_FILE 声明）
#include "GameTypes.h"   // cMyTrace / PrintBackTrace（[b28d-Main·检查点17] 模板定义入头后本头需要）

template <class T, int N>
class StaticPool
{
public:
    struct CNode : public T
    {
        // ORIG 各池 CNodeC1（CUser/CEnchanter/CMail/CACHE_CHARACTER_TYPE/_IO_FILE
        // 反汇编实证）：CNode 继承 T（多态，vptr 位于对象 +0，与 DynamicPool 同款
        // 布局；CEnchanter 池实证 vptr@+0、magic@0x48=sizeof(T)、bUse@0x4c），
        // T() 构造后 vptr 指向 CNode vtable（虚函数槽与 T 相同、dtor 槽为 CNode
        // 自身），随后写 magic=0x8f21、bUse=0。缺此构造则 magic/bUse 为垃圾值，
        // Acquire/Get/Free/GetIndex 的 magic==0x8f21 检查运行时失效。
        // 【b22 特批·终结】CACHE_CHARACTER_TYPE 池 CNodeC1（ORIG 0x8495dba，
        // 18 insns）实证指令序：memset(this,0,0x3f85c) → call T()C1(0x8492262)
        // → magic 写 → bUse 写。memset 先行于基 ctor 在 C++ 中不可复现（两难
        // 论证经 b22 复核成立）：① 基类 ctor 恒先于派生 ctor 体任何语句，无合法
        // 源表达式可插入先行 memset；② memset 置 ctor 体则反序抹掉 T() 七子对象
        // ctor 写入，语义更错。替代路径「memset-only extern 特化（不调 T()）」
        // 评估后否决：asm 仍 non-identical 且较现形多缺一个 ORIG 真 call；其
        // 语义压缩为纯零态仅因消费方 AcquireCharacter(0x8490a8a) 在 Acquire 后
        // 立即 memset(0x3f85c) 抹平而侥幸不可观测，非忠实建模；C++98 下嵌套类
        // 成员特化需动通用 CNode 声明、波及全部 19 池（StaticPool 事故判例）。
        // 现形（: T(), magic, bUse）= 距 ORIG 最近合法表达，残差固定 4 insns
        // （memset call 序列），按特批结案，勿再开轮。
        // b28c-L5（P216 应用·new[] cleanup 侧）：加 throw() 使 4.4 判该弱 ctor
        //   nothrow，从而**不再**为 `new CNode[N]` 的构造循环生成
        //   「异常回滚 → operator delete[]」EH 残段（实证：StaticPool<_IO_FILE,50>::C1
        //   @ORIG 0x084ba31a 全文无 `_ZdaPv` cleanup，我方 4.4 恒多 7 条
        //   `mov %edx,%esi; mov %eax,%edi; mov %ebx,(%esp); call _ZdaPv;
        //    mov %edi,%eax; mov %esi,%edx; jmp 尾`）。探针 /tmp/l5_probe_b.cpp
        //   SP<C>（CNode ctor 带 throw()）vs SP<A>（无）逐条对照：仅带 throw() 者
        //   无该残段，且 `call CNodeC1Ev` 不被折叠（P216「空体反例」边界不适用——
        //   本 ctor 体有 magic/bUse 两写，非纯空体）。
        // b28d-lane12（Main 批准方案①）：**不带** throw()。ORIG 非平凡析构 T 的池 C1
        // （CTradeSpace 0x082ae5ee 等 19 个）含完整 new[] EH cleanup
        // （0x82ae685..0x82ae6cd：CNodeD1 循环 + _ZdaPv + _Unwind_Resume）⇒ ORIG 的
        // CNode ctor 可抛。平凡析构的 _IO_FILE 池走下方全特化（保留 throw()）。
        CNode() : T(), magic(0x8f21), bUse(0) {}
        // ORIG 各池 CNode D1（非 POD T 的池如 CUser/CTradeSpace/CParty/
        // CDeathTower 实证 CNodeD1Ev W 符号）：隐式析构经 ~T() 链即满足，
        // 故不写用户定义析构——POD T 池（如 _IO_FILE）的 ORIG 不生成
        // CNodeD1Ev，用户定义空析构会让此类池多出逐元素析构循环。

        int magic;     // +sizeof(T)，恒 0x8f21
        char bUse;     // +sizeof(T)+4，引用计数
    };

    StaticPool();
    ~StaticPool();

    T* Acquire();
    T* Get(int index);
    void Free(T* p);
    int GetIndex(T* p);
    int GetLastErrorCode();   // ORIG 独立符号（非 inline 非 const）；定义见 CGameManager.cpp
    // b28c-lane3：ORIG Size 为**非静态**成员——全部调用点传 this
    //（ProcessServerStatusLog 0x080ede91 段 18 处 `add $off,%eax; mov %eax,(%esp);
    //  call SizeEv`；QuickParty::destroy 0x082695ca 段 `add $0x1cc,%eax`）。
    int Size() { return N; }
    int Remain() { return (int)m_Queue.size(); }

private:
    CNode* m_pNode;              // +0x00
    std::queue<CNode*> m_Queue;  // +0x04
    int m_nErrorCode;            // +0x2c
};

// ============================================================================
// 主模板成员定义（[b28d-Main·检查点17] 自 CGameManager.cpp 移入本头）
// 依据：ORIG 的 StaticPool 各成员是 **W 弱符号按需隐式实例化**（如
//   `StaticPool<BlueMarble,300>::Acquire` 0x082b4542 W），说明 ORIG 源码把定义放在
//   头里；我方此前「定义在 CGameManager.cpp + 19 条 `template class StaticPool<...>;`
//   显式实例化」会把**全部**成员（Get/Size/Remain/GetLastErrorCode）为 19 个类型
//   全部发射 ⇒ only-NEW 多出 66 个 ORIG 没有的符号（ORIG 仅
//   `StaticPool<CUser,600>::GetLastErrorCode` 1 个）。移入头后按使用点隐式实例化，
//   符号集与 ORIG 一致。成员体与 CGameManager.cpp 原定义逐字相同（含判词注释）。
// ============================================================================
template <class T, int N>
StaticPool<T, N>::StaticPool()
{
    m_pNode = new CNode[N];
    if (m_pNode == 0)
    {
        // ORIG C1 实证：trace 落浅槽 -0x30（push 临时对象占 -0x58），临时对象形态
        cMyTrace(__FUNCTION__, 0x6c, 5)("[StaticPool] %s(%d) new fail", "StaticPool.cpp", 0x6c);
    }
    for (int i = 0; i < N; ++i)
        m_Queue.push(&m_pNode[i]);
    m_nErrorCode = 0;
}

template <class T, int N>
StaticPool<T, N>::~StaticPool()
{
    // b28d-solo-CGameManager 判词：本模板实例三件（CTradeSpace<300> 0x082ae79c /
    // pvp_assault::CAssaultPlace<128> 0x082f141c / exchange_server::CSession<300>
    // 0x0849584c）均 N91 封顶——ORIG 37 insn 无 EH landing，4.4 恒发
    // CNodeD1 循环+_Unwind_Resume（P216 边界：D1 侧 throw() 无效，已实证）。
    // 语义侧 delete[] 的循环/cookie/释放三段与 ORIG 逐条一致，判 N91 封顶。
    delete[] m_pNode;
}

template <class T, int N>
T* StaticPool<T, N>::Acquire()
{
    if (m_Queue.empty())
        return 0;
    CNode* pNode = m_Queue.front();
    m_Queue.pop();
    if (pNode->magic != 0x8f21)
    {
        register int magic = pNode->magic;
        // ORIG 0x082b0351 实证：临时对象形态（非具名局部）→ magic trace 槽 -0x2c、
        // bUse trace 槽 -0x1c（具名写法槽序相反）
        cMyTrace(__FUNCTION__, 0x89, 0)("[StaticPool] magic(%d) node(%p)", magic, pNode);
        m_nErrorCode = 3;
    }
    if (pNode->bUse != 0)
    {
        register int use = pNode->bUse;
        cMyTrace(__FUNCTION__, 0x8f, 0)("[StaticPool] bUse(%d) node(%p)", use, pNode);
        m_nErrorCode = 4;
    }
    // ORIG `lea 0x1(%eax),%edx` = 后缀 ++（读改写单表达式，探针 t5 实证）
    pNode->bUse++;
    return pNode;
}

template <class T, int N>
T* StaticPool<T, N>::Get(int index)
{
    if (index < 0 || index > N - 1)
        return 0;
    CNode* pNode = &m_pNode[index];
    if (pNode->magic != 0x8f21)
    {
        register int magic = pNode->magic;
        // ORIG 0x82b0xxx 实证：临时对象形态使 magic trace 槽落 -0x2c、bUse 落 -0x1c
        cMyTrace(__FUNCTION__, 0xc3, 0)("[StaticPool] magic(%d) node(%p)", magic, pNode);
        m_nErrorCode = 7;
    }
    if (pNode->bUse != 1)
    {
        if (pNode->bUse != 0)
        {
            register int use = pNode->bUse;
            cMyTrace(__FUNCTION__, 0xca, 0)("[StaticPool] bUse(%d) node(%p)", use, pNode);
            m_nErrorCode = 8;
        }
    }
    return pNode;
}

template <class T, int N>
void StaticPool<T, N>::Free(T* p)
{
    // ORIG 0x082b082a 实证（探针 t8 全等）：null 检查块先于 pNode 声明、
    // trace 全临时对象且实参直接写表达式（无具名中间量）→
    // 槽序 trnull-0x38/pNode-0x3c/trm-0x28/trb-0x18 + %esi 前置装载 + -- 单装载尾
    if (p == 0)
        cMyTrace(__FUNCTION__, 0xa0, 5)("[StaticPool] %s(%d) free null", "StaticPool.cpp", 0xa0);
    CNode* pNode = (CNode*)p;
    m_Queue.push(pNode);
    if (pNode->magic != 0x8f21)
    {
        cMyTrace(__FUNCTION__, 0xaa, 0)("[StaticPool] magic(%d) node(%p)", pNode->magic, pNode);
        m_nErrorCode = 5;
    }
    if (pNode->bUse != 1)
    {
        cMyTrace(__FUNCTION__, 0xb0, 0)("[StaticPool] bUse(%d) node(%p)", pNode->bUse, pNode);
        m_nErrorCode = 6;
        PrintBackTrace();
    }
    pNode->bUse--;
}

template <class T, int N>
int StaticPool<T, N>::GetIndex(T* p)
{
    if ((unsigned int)p < (unsigned int)m_pNode)
        return -1;
    int diff = (int)((char*)p - (char*)m_pNode);
    int index = 0;
    if (diff != 0)
        index = (unsigned int)diff / sizeof(CNode);
    CNode* pNode = (CNode*)p;
    if (pNode->magic != 0x8f21)
    {
        register int magic = pNode->magic;
        // ORIG 实证：临时对象形态 → magic trace 槽 -0x34、bUse 槽 -0x24（具名相反）
        cMyTrace(__FUNCTION__, 0x50, 0)("[StaticPool] magic(%d) node(%p)", magic, pNode);
        m_nErrorCode = 1;
    }
    if (pNode->bUse != 1)
    {
        register int use = pNode->bUse;
        cMyTrace(__FUNCTION__, 0x56, 0)("[StaticPool] bUse(%d) node(%p)", use, pNode);
        m_nErrorCode = 2;
    }
    if (index > N - 1)
        return -1;
    return index;
}

template <class T, int N>
int StaticPool<T, N>::GetLastErrorCode()
{
    return m_nErrorCode;
}

// ============================================================================
// StaticPool<_IO_FILE, 50> 全特化（b28d-lane12，Main 2026-09-08 批准方案①）
// 依据：ORIG `StaticPool<_IO_FILE,50>::C1` 0x084ba31a（99 insn）**无** new[]
//   EH cleanup 段（元素 _IO_FILE 析构平凡）；而模板版 CNode ctor 不带 throw() 时
//   4.4 会多发 8 条 `CNodeD1` 循环 + `_ZdaPv` 回滚（99→107、ndiff 26 实测）。
//   非平凡析构 T 的 19 个池则相反：ORIG 有完整 cleanup（CTradeSpace C1 0x082ae5ee
//   0x82ae685..0x82ae6cd 实证），throw() 会消掉它（ndiff 51→0 实测）。
// 故仅本池保留 `CNode() throw()`；其余成员体与主模板逐字相同（N→50）。
// 成员函数定义见 CGameManager.cpp（显式特化形式）；符号名与模板实例相同
// （_ZN10StaticPoolI8_IO_FILELi50EE*），sizeof/布局/mangled 无变化。
// ============================================================================
template <>
class StaticPool<_IO_FILE, 50>
{
public:
    struct CNode : public _IO_FILE
    {
        CNode() throw() : _IO_FILE(), magic(0x8f21), bUse(0) {}
        int magic;     // +sizeof(_IO_FILE)
        char bUse;     // +sizeof(_IO_FILE)+4
    };

    StaticPool();
    ~StaticPool();

    _IO_FILE* Acquire();
    _IO_FILE* Get(int index);
    void Free(_IO_FILE* p);
    int GetIndex(_IO_FILE* p);
    int GetLastErrorCode();
    int Size() { return 50; }
    int Remain() { return (int)m_Queue.size(); }

private:
    CNode* m_pNode;              // +0x00
    std::queue<CNode*> m_Queue;  // +0x04
    int m_nErrorCode;            // +0x2c
};

// 布局与主模板一致（CNode* + std::queue + int = 0x30；ORIG logfiles_@0x0943dd00 /
// count_@0x0943dd30 差值 0x30 实证）。
static_assert(sizeof(StaticPool<_IO_FILE, 50>) == 0x30, "StaticPool<_IO_FILE,50> ABI drift");

#endif  // GAME_STATICPOOL_H_
