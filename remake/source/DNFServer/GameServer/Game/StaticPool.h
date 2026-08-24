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
    struct CNode
    {
        T data;        // +0x00（sizeof(T) 字节）
        int magic;     // +sizeof(T)，恒 0x8f21
        char bUse;     // +sizeof(T)+4，引用计数
    };

    StaticPool();
    ~StaticPool();

    T* Acquire();
    T* Get(int index);
    void Free(T* p);
    int GetIndex(T* p);
    int GetLastErrorCode() const { return m_nErrorCode; }
    static int Size() { return N; }
    int Remain() { return (int)m_Queue.size(); }

private:
    CNode* m_pNode;              // +0x00
    std::queue<CNode*> m_Queue;  // +0x04
    int m_nErrorCode;            // +0x2c
};

#endif  // GAME_STATICPOOL_H_
