#ifndef GAME_STATICPOOL_H_
#define GAME_STATICPOOL_H_

// ============================================================================
// StaticPool<T,N> —— ORIG 真实实现（objdump 0x82ae390 起逐函数核对）
// （2026-08-23 自 CGameManager.h 提取为独立叶子头，供 CMailBoxHelper.h 等
//   轻量消费者使用；实现见 CGameManager.cpp。）
// ============================================================================

#include <queue>

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
    static int Size() { return N; }
    int Remain() { return (int)m_Queue.size(); }

private:
    CNode* m_pNode;              // +0x00
    std::queue<CNode*> m_Queue;  // +0x04
    int m_nErrorCode;            // +0x2c
};

#endif  // GAME_STATICPOOL_H_
