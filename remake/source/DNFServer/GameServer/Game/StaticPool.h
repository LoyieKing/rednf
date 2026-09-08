#ifndef GAME_STATICPOOL_H_
#define GAME_STATICPOOL_H_

// ============================================================================
// StaticPool<T,N> —— ORIG 真实实现（objdump 0x82ae390 起逐函数核对）
// （2026-08-23 自 CGameManager.h 提取为独立叶子头，供 CMailBoxHelper.h 等
//   轻量消费者使用；实现见 CGameManager.cpp。）
// ============================================================================

#include <queue>
#include <cstdio>   // b28d-lane12：_IO_FILE 全特化需完整类型（struct _IO_FILE 声明）

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
