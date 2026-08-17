#ifndef GAME_CSERVERPROXYMGR_H_
#define GAME_CSERVERPROXYMGR_H_

// ============================================================================
// df_game_r 还原 —— CServerProxyMgr<T> 代理管理模板（GlobalData TU 实例化）。
// 布局（sizeof = 0x10 + sizeof(T)）：
//   +0x00 int*  m_pIndexArray  （按 index 记录 ENUM_SERVER_GROUP 的 int 数组）
//   +0x04 T*    m_pProxyArray  （动态代理数组，cookie 在 [-4]）
//   +0x08 int   m_count
//   +0x0c char  m_allocated
//   +0x10 T     m_proxy        （内嵌默认代理）
// 动态数组元素步长为 sizeof(T)-0x10（ORIG createServerProxy 分配
//   count*(sizeof(T)-0x10)+4 字节，元素自 +4 起按该步长排布；析构自
//   尾向首销毁 count-1..1 号元素，首元素不销毁——ORIG 原样复刻）。
// 方法均为弱符号，在 GlobalData TU 隐式实例化（0x82b314e/0x82b3212 等）。
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
    T* createServerProxy(int count, int index, ENUM_SERVER_GROUP group,
                         char* ip, int tcpPort, int udpPort);

private:
    int* m_pIndexArray;   // +0x00
    T* m_pProxyArray;     // +0x04
    int m_count;          // +0x08
    char m_allocated;     // +0x0c
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
CServerProxyMgr<T>::~CServerProxyMgr()
{
    if (m_allocated)
    {
        if (m_pIndexArray)
        {
            delete[] m_pIndexArray;
        }
        m_pIndexArray = 0;
        if (m_pProxyArray)
        {
            int n = *((int*)m_pProxyArray - 1);
            char* p = (char*)m_pProxyArray + n * (sizeof(T) - 0x10);
            while (p != (char*)m_pProxyArray)
            {
                p -= (sizeof(T) - 0x10);
                ((T*)p)->~T();
            }
            delete[] ((char*)m_pProxyArray - 4);
        }
        m_pProxyArray = 0;
    }
    m_count = 0;
    m_proxy.~T();
}

template <class T>
int CServerProxyMgr<T>::GetStartIndex()
{
    if (m_count > 0)
    {
        if (m_allocated)
        {
            return m_pIndexArray[0];
        }
    }
    return 0;
}

template <class T>
int CServerProxyMgr<T>::GetEndIndex()
{
    return m_count;
}

template <class T>
int CServerProxyMgr<T>::GetNextIndex(int& idx)
{
    idx = idx + 1;
    if (m_allocated)
    {
        if (m_count > idx)
        {
            return m_pIndexArray[idx];
        }
    }
    return 0;
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
                return (T*)((char*)m_pProxyArray + i * (sizeof(T) - 0x10));
            }
        }
    }
    return &m_proxy;
}

template <class T>
T* CServerProxyMgr<T>::GetServerProxyArrayAt(int idx)
{
    if (idx < 0)
    {
        return 0;
    }
    if (m_count <= idx)
    {
        return 0;
    }
    return (T*)((char*)m_pProxyArray + idx * (sizeof(T) - 0x10));
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
        char* buf = new char[count * (sizeof(T) - 0x10) + 4];
        *(int*)buf = count;
        char* p = buf + 4;
        for (int i = count - 1; i != -1; i--)
        {
            new ((T*)p) T();
            p += sizeof(T) - 0x10;
        }
        m_pProxyArray = (T*)(buf + 4);
        m_count = count;
    }
    m_pIndexArray[index] = (int)group;
    T* t = (T*)((char*)m_pProxyArray + index * (sizeof(T) - 0x10));
    t->Init(std::string(ip), tcpPort, udpPort);
    return t;
}

#endif  // GAME_CSERVERPROXYMGR_H_
