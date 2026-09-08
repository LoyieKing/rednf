#ifndef GAME_CSERVERPROXYMGR_H_
#define GAME_CSERVERPROXYMGR_H_

// ============================================================================
// df_game_r 还原 —— CServerProxyMgr<T> 代理管理模板（GlobalData TU 实例化）。
// 布局（sizeof = 0x10 + sizeof(T)）：
//   +0x00 int*  m_pIndexArray  （按 index 记录 ENUM_SERVER_GROUP 的 int 数组）
//   +0x04 T*    m_pProxyArray  （动态代理数组，cookie 在 [-4]）
//   +0x08 int   m_count
//   +0x0c bool  m_allocated
//   +0x10 T     m_proxy        （内嵌默认代理）
// 动态数组元素步长为 sizeof(T)（ORIG createServerProxy 分配
//   count*sizeof(T)+4 字节，元素自 +4 起按 sizeof(T) 排布；析构自
//   尾向首销毁全部元素）。Monitor 步长 0x30e0c / Guild 0x30e24 /
//   Statistic 0x10，均等于各自 sizeof(T)（ORIG new mgr 尺寸 0x30e1c/
//   0x30e34/0x20 减 0x10 证实，createServerProxy/D1/GetServerProxy/
//   GetServerProxyArrayAt 的 imul 立即数一致）。
// 方法均为弱符号，在 GlobalData TU 显式/消费 TU 隐式实例化
// （C1 0x82b314e/0x82b3212/0x82b3296、D1 0x82b39fe/0x82b3ac6/0x82b3b8e 等）。
// ============================================================================

#include <string>

#include "CEnvironment.h"

template <class T>
class CServerProxyMgr
{
public:
    CServerProxyMgr();
    ~CServerProxyMgr();

    int GetStartIndex();
    int GetEndIndex();
    int GetNextIndex(int& idx);
    T* GetServerProxy(ENUM_SERVER_GROUP group);
    T* GetServerProxyArrayAt(int idx);
    int GetServerProxyArraySize();
    T* createServerProxy(int count, int index, ENUM_SERVER_GROUP group,
                         char* ip, int tcpPort, int udpPort);

private:
    int* m_pIndexArray;   // +0x00
    T* m_pProxyArray;     // +0x04
    int m_count;          // +0x08
    bool m_allocated;     // +0x0c
    char m_pad[3];        // +0x0d
    T m_proxy;            // +0x10
};

template <class T>
CServerProxyMgr<T>::CServerProxyMgr()
{
    m_pIndexArray = 0;
    m_pProxyArray = 0;
    m_count = 0;
    m_allocated = 0;
    m_proxy.Init();
}

template <class T>
// [b28d-solo-GlobalData] CServerProxyMgr<T>::~CServerProxyMgr 三实例
// （CStatisticServerProxy 0x082b3b8e / CGuildServerProxy 0x082b3ac6 /
//  CMonitorServerProxy 0x082b39fe）：删显式 m_proxy.~T()（ORIG 仅自动析构，
// 原为双重析构真 bug）+ register p（P231）；T* 指针算术（P211 档③）+
// goto 循环形（探针 /tmp/probe_loop.cpp f2 复现 4.1 条件头/尾 jmp 布局）+
// 显式 ::operator delete[]（P207 消 4.4 判空守卫）：ndiff 19→1。
// 余 1 条 = ORIG `cmp %eax,%ebx` vs 4.4 `cmp %ebx,%eax`（比较方向恒折，
// 交换操作数实测无效）⇒ N18b 封顶。
CServerProxyMgr<T>::~CServerProxyMgr()
{
    if (m_allocated)
    {
        delete[] m_pIndexArray;
        m_pIndexArray = 0;
        if (m_pProxyArray)
        {
            // ORIG D1（0x082b3ac6 族）：`mov 0x4(%eax),%edx` 单段基址 + `imul
            // $sizeof(T); lea (%edx,%eax,1),%ebx`（T* 缩放寻址）+ `cmp %eax,%ebx;
            // je` 直接指针比较（无 setne 物化）+ `mov %ebx,(%esp); call ~T()`。
            // 用 char* 手工 *sizeof(T) 会引入 `mov %eax,%edx` 中转与 `p != base`
            // 的 setne 物化 ⇒ 改 T* 指针算术（P211 档③ 同型直落 + P292 直比较）。
            register T* p = m_pProxyArray + *((int*)m_pProxyArray - 1);
            // 4.4 对 `while (p != base)` 恒旋转 + setne 物化（探针
            // /tmp/probe_loop.cpp f/g/h/k/m）；goto 形（f2）精确复现 ORIG
            // 「条件头 fall-through body + 尾 jmp 回条件」布局且无 setne。
        L_destroy:
            if (p == m_pProxyArray)
                goto L_destroy_end;
            --p;
            p->~T();
            goto L_destroy;
        L_destroy_end:;
            // ORIG `sub $0x4,%eax; mov %eax,(%esp); call _ZdaPv` 无判空
            // （4.1 delete[] 无守卫）；4.4 `delete[]` 自带 test/je ⇒ 改显式
            // operator delete[] 调用复现（P207）。
            ::operator delete[]((char*)m_pProxyArray - 4);
        }
        m_pProxyArray = 0;
    }
    m_count = 0;
}

template <class T>
int CServerProxyMgr<T>::GetStartIndex()
{
    // ORIG 0x82b44ba-0x82b44e4：`test; jle [mov$0;jmp 岛]` +
    // `movzbl 0xc(%eax),%eax; xor $0x1,%eax; test; je load` ⇒ 两级 early-return，
    // !m_allocated 用 bool 成员取反（xor$1 物化形，技巧1 同族）
    if (0 < m_count)
    {
        if (!m_allocated)
        {
            goto RET0;
        }
        goto LOAD;
    }
RET0:
    return 0;
LOAD:
    return m_pIndexArray[0];
}

template <class T>
int CServerProxyMgr<T>::GetEndIndex()
{
    return m_count;
}

template <class T>
int CServerProxyMgr<T>::GetNextIndex(int& idx)
{
    // ORIG 0x82b450x 族：`lea 0x1(%eax),%edx; mov %edx,(%eax)` 先自增落参；
    // `movzbl 0xc; xor $0x1; test; je` + 假路 fall `mov$0;jmp`（!m_allocated）；
    // `cmp %eax,%edx; jg BODY; mov$0;jmp` ⇒ !(m_count>idx) 早退，真体最后
    idx = idx + 1;
    if (!m_allocated)
    {
        return 0;
    }
    if (!(m_count > idx))
    {
        return 0;
    }
    return m_pIndexArray[idx];
}

template <class T>
T* CServerProxyMgr<T>::GetServerProxy(ENUM_SERVER_GROUP group)
{
    if (group == E_SERVER_GROUP_NONE)
    {
        return &m_proxy;
    }
    for (int i = 0; i < m_count; i++)
    {
        if (m_pIndexArray)
        {
            if (m_pIndexArray[i] == (int)group)
            {
                return (T*)((char*)m_pProxyArray + i * sizeof(T));
            }
        }
    }
    return &m_proxy;
}

template <class T>
T* CServerProxyMgr<T>::GetServerProxyArrayAt(int idx)
{
    if (idx < 0 || m_count <= idx)
    {
        return 0;
    }
    return (T*)((char*)m_pProxyArray + idx * sizeof(T));
}

// ORIG _ZN15CServerProxyMgrI19CMonitorServerProxyE23GetServerProxyArraySizeEv
// W 0x0857f8c2：读 m_count（MonitorTcpThread::dispatch 轮询用，按需实例化）。
template <class T>
int CServerProxyMgr<T>::GetServerProxyArraySize()
{
    return m_count;
}

template <class T>
T* CServerProxyMgr<T>::createServerProxy(int count, int index,
                                         ENUM_SERVER_GROUP group,
                                         char* ip, int tcpPort, int udpPort)
{
    if (group == E_SERVER_GROUP_NONE)
    {
        m_pProxyArray = &m_proxy;
        m_count = 1;
        m_proxy.Init(std::string(ip), tcpPort, udpPort);
        return &m_proxy;
    }
    if (!m_allocated)
    {
        m_allocated = 1;
        m_pIndexArray = new int[count];
        char* buf = new char[count * sizeof(T) + 4];
        *(int*)buf = count;
        char* p = buf + 4;
        for (int i = count - 1; i != -1; i--)
        {
            new ((T*)p) T();
            p += sizeof(T);
        }
        m_pProxyArray = (T*)(buf + 4);
        m_count = count;
    }
    m_pIndexArray[index] = (int)group;
    T* t = (T*)((char*)m_pProxyArray + index * sizeof(T));
    t->Init(std::string(ip), tcpPort, udpPort);
    return t;
}

#endif  // GAME_CSERVERPROXYMGR_H_
